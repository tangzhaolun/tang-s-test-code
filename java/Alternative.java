import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.io.IOException;

//import javax.activation.MimetypesFileTypeMap;

public class Alternative {
	
//	static void detectAviFile2(String path) {
//		// MimetypesFileTypeMap only supports checking file suffix, not meta-data inside the file
//		// So it's not recommended. 
//		
//		if (path.isEmpty()) path = "/media/SONY 4G";	//default directory to store avi files
//		String aviAlias[] = {"video/x-msvideo", "video/x-avi", "video/avi", "video/vnd.divx"};
//		File f = new File(path);
//
//		if (f.isDirectory()){
//			String files[];
//			files = f.list();
//			//detect each file in the given directory
//			MimetypesFileTypeMap mime = new MimetypesFileTypeMap();
//			mime.addMimeTypes("video/x-msvideo avi");
//			for (int i = 0; i < files.length; i++){
//				String type = mime.getContentType(f.getPath() + "/" + files[i]);
//				//System.out.println(files[i] + ":" + type);
//				for (String s: aviAlias){
//						if (type.startsWith(s)){
//							System.out.println(files[i] + " is avi file.");
//							if (!files[i].endsWith("avi"))
//								System.out.println("\tWarning: file suffix may be wrong.");
//						break;
//						}
//				}
//			} //end of for
//		}
//		else {
//			System.out.println("It is not a directory."); 
//		}
//	
//	}
	
	static void detectAviFile(String path){
		// TODO only support 'avi', lack test source of mpeg4, divx, xvid
		
		if (path.isEmpty()) path = "/media/SONY 4G";	//default directory to store avi files
		//com[0]-command name; com[1]-command options; com[2]-target test file;
		//optional command: gst-typefind-0.10
		String com[] = {"file","-bi", ""};
		
		String aviAlias[] = {"video/x-msvideo", "video/x-avi", "video/avi", "video/vnd.divx"};

		File f = new File(path);
		if (f.isDirectory()){
			String files[];
			files = f.list();
			//detect each file in the given directory
			for (int i = 0; i < files.length; i++){
				try {
					com[2] = f.getPath() + "/" + files[i];
					Process p = Runtime.getRuntime().exec(com);
					
					BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
					String res; //parse result from executing file command
					if ((res = stdInput.readLine()) != null) {
						for (String s: aviAlias){
							if (res.startsWith(s)){
								System.out.println(files[i] + " is avi file.");
								if (!files[i].endsWith("avi"))
									System.out.println("\tWarning: file suffix may be wrong.");
							break;
							}
						}
						//System.out.println(res);
					}
		               
				}catch (IOException e) {
					e.printStackTrace();	
					System.exit(-1);
				} //end of try
			} //end of for
		}
		else {
			System.out.println("It is not a directory."); 
		}

	}
	
	public static void main(String[] args) {
		detectAviFile("");
	}
}