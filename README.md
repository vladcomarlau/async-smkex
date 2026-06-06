# Async-SMKEX

An **asynchronous, offline-capable authentication layer for SMKEX**. It lets a
recipient be **offline** during key exchange while preserving SMKEX's split-trust,
multipath security: an offline recipient's PreKey bundle is authenticated over
**two independent network paths** (a PreKey server and a Notary server) instead of
being vouched for by a single server.

> Master's research prototype — Advanced Cybersecurity (Year 1, Semester 2),
> Faculty of Automatic Control and Computers, University POLITEHNICA of Bucharest.
> Author: Vlad-Constantin Comârlău. Scientific coordinator: Assoc. Prof. Dr. Eng.
> Marios O. Choudary.
>
> ⚠️ Research prototype, not production software. The demo endpoints are
> intentionally unauthenticated.

## What this adds

- **PreKey server (path α)** and **Notary server (path β)** — new REST endpoints on
  the SMKEX server, run as two independent instances. Bundle storage (α) is kept
  separate from the signed-hash attestations (β), preserving the split-trust idea.
- **Per-one-time-PreKey attestation** — every one-time PreKey is individually
  signed (X3DH leaves them unsigned).
- **`asyncclient/`** — a standalone C++ client doing registration, retrieval +
  verification, and X3DH key agreement, with a local SQLite store. Both the two
  registration POSTs and the two retrieval GETs run **concurrently** over the two
  paths.

## Requirements

- **Docker** — for the two servers.
- **WSL or Linux** with `build-essential`, `libssl-dev`, `libsqlite3-dev`,
  `libcurl4-openssl-dev` — for the client.

## Running the demo

### 1. Start the two servers (Docker)

```bash
# The PreKey and Notary servers are the same app; build it once.
docker build -t smkex-async smkex-server/smkex-application-server

# Run two instances = the two independent paths.
docker run -d -p 8081:8081 --name prekey smkex-async   # PreKey server (path alpha)
docker run -d -p 8082:8081 --name notary smkex-async   # Notary server (path beta)
```

### 2. Build the client (WSL / Linux)

```bash
sudo apt install -y build-essential libssl-dev libsqlite3-dev libcurl4-openssl-dev
cd asyncclient
make
```

### 3. Run the asynchronous flow

```bash
rm -f bob.db alice.db          # start fresh (init won't regenerate over an existing identity)

# Bob registers his keys, then can go OFFLINE.
./aclient bob.db init          # copy the printed "Identity (Ed25519): ..." value
./aclient bob.db register bob

# Alice sends an encrypted message while Bob is offline.
./aclient alice.db init
./aclient alice.db send alice bob <BOBS_Ed25519_KEY> "Hello Bob - async SMKEX works!"

# Bob comes back ONLINE and decrypts.
./aclient bob.db fetch bob
```

`send` and `fetch` each print a `session key SK`; the two values are **identical**,
proving both sides derived the same key while Bob was offline.

**Split-trust security demo** — retrieving with a wrong pinned identity key is
rejected (the second path catches the tampering):

```bash
./aclient alice.db retrieve bob <ANY_WRONG_KEY>     # => BUNDLE REJECTED
```

Stop and remove the servers when done:

```bash
docker rm -f prekey notary
```

## Credit and license

This project **builds on the original SMKEX end-to-end encryption project** by
University POLITEHNICA of Bucharest and CertSIGN:

- Original repository: **end2end-smkex** — <https://gitlab.cs.pub.ro/marios.choudary/end2end-smkex>
- SMKEX protocol: S. Costea, M. O. Choudary, et al., *Secure Opportunistic
  Multipath Key Exchange*, ACM CCS 2018.

The upstream code (the `MultiPathVoIPLite/`, `smkex-server/`, `android-client/`,
`pjproject-2.10/` directories and others) is licensed under the **BSD 3-Clause
License, Copyright © 2020 Marios O. Choudary** — see [LICENSE](LICENSE_SMKEX). All rights
to the original work remain with its authors. The asynchronous extension in this
repository is an independent academic research contribution built on top of it.
