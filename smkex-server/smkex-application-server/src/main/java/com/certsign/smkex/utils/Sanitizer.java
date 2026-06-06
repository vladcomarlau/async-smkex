package com.certsign.smkex.utils;

import java.net.URI;

import com.certsign.smkex.model.Message;

public abstract class Sanitizer {

	public static boolean isStringEmpty(String str) {
		return str == null || str.trim().length() == 0;
	}

	public static boolean isMessageValid(Message message) {
		return message != null 
				&& !isStringEmpty(message.getContent()) 
				&& !isStringEmpty(message.getSenderSerial());
	}

	public static String getUserSerialFromURI(URI uri) {
		String path = uri.getPath();
		String userSerial = path.substring(path.lastIndexOf("/") + 1);

		return userSerial;
	}

}
