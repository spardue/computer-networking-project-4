package clientlogic;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.List;


/**
 * 
 * @author Stephen Pardue
 *
 */


public class ClientLogic {
	
	public final int SERVER_PORT = 1337;
	public final String SERVER_ADDRESS = "128.61.24.142";
	//public final String LOCAL_FILE_PATH = "files";
	
	private Socket s;
	private OutputStream out;
	public DataInputStream is;
	private File root; //root directory for external storage
	
	public ClientLogic(File root){
		this.root = root;
		
		try {
			s = new Socket(SERVER_ADDRESS, SERVER_PORT);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		try {
			out = s.getOutputStream();
			is = new DataInputStream(s.getInputStream());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
	public List<FileInfo> list(){
		List<FileInfo> ret = null;
		
		try {
			out.write('L');
			ret = getFileListFromServer();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return ret;
	}
	
	public List<FileInfo> diff(){
		List<FileInfo> ret = null;
		List<FileInfo> serverFiles;
		List<FileInfo> localFiles =  getFilesFromLocalHost();
		
		try {
			out.write('D');
			serverFiles = getFileListFromServer();
			serverFiles.removeAll(localFiles);
			ret = serverFiles;
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		return ret;
	}
	
	
	
	
	public void pull(){
		List<FileInfo> ret = null;
		List<FileInfo> serverFiles;
		List<FileInfo> localFiles =  getFilesFromLocalHost();
		
		try {
			out.write('P');
			serverFiles = getFileListFromServer();
			//dif
			serverFiles.removeAll(localFiles);
			
			//send the amount
			out.write((char) serverFiles.size());
			if (serverFiles.size() > 0){
				for (FileInfo file : serverFiles){
					sendFileInfo(file);
				}
			}
			System.out.println("\nSent the file Info");
			for (int i = 0; i < serverFiles.size(); i++){
				recvFile();
			}
			
			
		} catch (IOException e){
			e.printStackTrace();
		}
	}
	
	
	public void exit(){
		try {
			out.write('E');
			out.close();
			is.close();
			s.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void recvFile(){
		
		FileInfo fInfo = new FileInfo();
		for (int i = 0; i < FileInfo.FILE_MAX; i++){
			try {
				char buff = (char) is.read();
				if (buff != '\0'){
					fInfo.name += buff;
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		//System.out.println("bob : "+fInfo);
		int size = -1;
		try {
			size = is.readInt();
			size = Integer.reverseBytes(size);
			//System.out.println(size);
			File newFile = new File(root.getAbsolutePath(), fInfo.name); 
			System.out.println(newFile+" "+size);
			RandomAccessFile f = new RandomAccessFile(newFile, "rw");
			for (int i = 0; i < size; i++){
				f.write(is.readByte());
			}
			f.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
	private void sendFileInfo(FileInfo file){
		
		try {
			//C struct padding
			//for (int i = 0; i < 3; i++) {
			//	out.write(0);
			//}
			
			

			
			byte name[] = file.name.getBytes();
			for (int i = 0; i < name.length; i++){
				out.write(name[i]);
				System.out.print((char) name[i]);
			}
			
			//out.write('k');
			//out.write('\n');
			int zeros = FileInfo.FILE_MAX - name.length;
			System.out.println(zeros);
			for (int i = 0; i < zeros; i++){
				out.write((byte) '\0');
			}
			
			for (int i = 0; i < FileInfo.CHECKSUM_LENGTH; i++){
				out.write(file.checksum[i]);
			}
			
//			for (int i = 0; i < FileInfo.NEXT_POINTER_LENGTH; i++){
//				out.write(file.nextPointer[i]);
//			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	
	
	private List<FileInfo> getFilesFromLocalHost(){
		List<FileInfo> ret = new ArrayList<FileInfo>();
		byte buff[];
		
		//File root = new File(LOCAL_FILE_PATH);
		
		try {
			if (! root.exists()) {
				root.createNewFile();
			}
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		for (File file :  root.listFiles()) {
			FileInfo fInfo = new FileInfo();
			try {
				RandomAccessFile f = new RandomAccessFile(file, "r");
				buff = new byte[(int) f.length()];
				f.readFully(buff);
				MessageDigest md = MessageDigest.getInstance("MD5");
				
				
				fInfo.checksum = md.digest(buff);
				fInfo.name = file.getName();
				ret.add(fInfo);
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (NoSuchAlgorithmException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return ret;
	}
	
	
	private List<FileInfo> getFileListFromServer(){
		List<FileInfo> ret = new ArrayList<FileInfo>();
		try {
			int length = is.readByte();
			
			//System.out.println(length);
			for (int i = 0; i < length; i++){
				ret.add(readFileInfo());
			}
			//read padding
			for (int i = 0; i < 3; i++){
				is.readByte();
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return ret;
	}
	
	
	
	
	
	private FileInfo readFileInfo(){
		FileInfo ret = new FileInfo();
		
		int count = 0;
		
		try {
			//for the C struct padding
			for (int i = 0; i < 3; i++){
				is.readByte();
				count++;
			}
			
			for (int i = 0; i < FileInfo.FILE_MAX; i++){
				char buff = (char) is.readByte();
				count++;
				if (buff != '\0'){
					ret.name += buff;
				}
			}
			
			for (int i = 0; i < FileInfo.CHECKSUM_LENGTH; i++){
				byte buff = is.readByte();
				count++;
				ret.checksum[i] = (byte) (buff & 0xff);
			}
			
			for (int i = 0; i < FileInfo.NEXT_POINTER_LENGTH; i++){
				ret.nextPointer[i] = is.readByte();
				count++;
			}
			//System.out.println("count "+count);
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return ret;
	}
	
	
	public static void main(String[] arg){
		
//		ClientLogic logic = new ClientLogic();
//		System.out.println(logic.list());
//		System.out.println(logic.list());
//		//System.out.println(logic.getFilesFromLocalHost());
//		System.out.println(logic.diff());
//		logic.pull();
//		logic.exit();
		
	}
	
}
