package com.example.cs_3251_project_3;

public class FileInfo {
	public static final int FILE_MAX = 255;
	public static final int CHECKSUM_LENGTH = 16;
	public static final int NEXT_POINTER_LENGTH = 14;
	public String name = "";
	public byte[] checksum = new byte[CHECKSUM_LENGTH];
	public byte[] nextPointer = new byte[NEXT_POINTER_LENGTH];
	
	public String toString(){
		String ret = "{Name: "+ new String(name) + ", Checksum: ";
		
		for (int i = 0; i < checksum.length; i++){
			ret+=ret.format("%02x", checksum[i]);
		}
		ret+="}";
		
		return ret;
	}
	
	public boolean equals(Object other){
		FileInfo fInfo = (FileInfo) other;
		
		for (int i = 0; i < CHECKSUM_LENGTH; i++){
			if (fInfo.checksum[i] != checksum[i]){
				return false;
			}
		}
		return true;
	}
}
