package com.certsign.smkex.security;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.authentication.builders.AuthenticationManagerBuilder;
import org.springframework.security.config.annotation.method.configuration.EnableGlobalMethodSecurity;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;

import com.certsign.smkex.utils.SmkexProperties;

@Configuration
@EnableWebSecurity
@EnableGlobalMethodSecurity(prePostEnabled = true)
public class SecurityConfig extends WebSecurityConfigurerAdapter {

	private Logger LOG = LoggerFactory.getLogger(SecurityConfig.class);

	@Autowired
	private AuthenticationEntryPoint entryPoint;
	@Autowired
	private SmkexProperties smkexProperties;

	@Override
	protected void configure(HttpSecurity http) throws Exception {
		LOG.info("Configure security rules for paths: {}, {}", smkexProperties.getPublicZone(),
				smkexProperties.getPrivateZone());
		
		http
				/* No authentication for /public end-points */
				.authorizeRequests().antMatchers(smkexProperties.getPublicZone()).permitAll()

				/* Use basic authentication for other end-points */
				.antMatchers(smkexProperties.getPrivateZone()).authenticated().and().httpBasic()
				.authenticationEntryPoint(entryPoint);

		/* Disable CSRF protection */
		http.cors().and().csrf().disable();
	}

	@Autowired
	public void configureGlobal(AuthenticationManagerBuilder auth) throws Exception {
		auth.inMemoryAuthentication().withUser(smkexProperties.getAuthUsername())
				.password(passwordEncoder().encode(smkexProperties.getAuthPassword())).authorities("ROLE_USER");
	}

	@Bean
	public PasswordEncoder passwordEncoder() {
		return new BCryptPasswordEncoder();
	}
}
