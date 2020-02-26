package pattern_match;

import java.lang.String;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Stack;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


class Context{
	public	int pat_end;
	public int content_end;
	public int type;   // 0 = normal ; 1=*; 2=**
}


public class PatternMatch {

	static char esc_str = '.';
	public static Pattern convertGlobToRegex(String globPattern) {
        StringBuilder sb = new StringBuilder(globPattern.length());
        char[] arr = globPattern.toCharArray();
        for (int i = 0; i < arr.length; i++) {
            char ch = arr[i];
            switch (ch) {
                case '*':
                   //case '**'
                   if (i + 1 < arr.length) {
                       if (arr[i + 1] == '*') {
                           sb.append(".*");
                           i++;
                           break;
                       }
                   }
                   sb.append("[^.\\]\\[]*");
                   break;
                case '.':
                   sb.append("\\.");
                   break;
                case '[':
                    sb.append("\\[");
                    break;
                case ']':
                    sb.append("\\]");
                    break;
                default:
                    sb.append(ch);
            }
        }

        String regex = sb.toString();
//        System.out.println(regex);
        try {
            Pattern pattern = Pattern.compile(regex);
            return pattern;
        } catch(IllegalArgumentException e) {
            return null;
        }
	}
	
	 public static boolean matchPattern(Pattern pattern, String value) {
	        if (pattern == null || value == null) {
	            return false;
	        }
	        Matcher matcher = pattern.matcher(value);
	        return matcher.matches();
	    }

	 	 
	 public static String[] readLines(String filename) {
	        FileReader fileReader;
	        List<String> lines = new ArrayList<String>();
			try {
				fileReader = new FileReader(filename);
	
	        BufferedReader bufferedReader = new BufferedReader(fileReader);
	        
	        String line = null;
	        while ((line = bufferedReader.readLine()) != null) {
	            lines.add(line);
	        }
	        bufferedReader.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	        return lines.toArray(new String[lines.size()]);
	    }
	 
	 public static boolean stringCompare(final String str1, final String str2) 
	    { 
	  
	        final int l1 = str1.length(); 
	        final int l2 = str2.length(); 
	        int lmin = Math.min(l1, l2); 
	  
	        for (int i = 0; i < lmin; i++) { 
	            int str1_ch = (int)str1.charAt(i); 
	            int str2_ch = (int)str2.charAt(i); 
	  
	            if (str1_ch != str2_ch) { 
	                return false;
	            } 
	        } 
	  
	        // Edge case for strings like 
	        // String 1="Geeks" and String 2="Geeksforgeeks" 
	        if (l1 != l2) { 
	            return false; 
	        } 	  
	        // If none of the above conditions is true, 
	        // it implies both the strings are equal 
	        else { 
	            return true; 
	        } 
	    } 
	 
	 
	 public static  boolean quickMatchPattern(final String pattern, final String content) {		
		 if (pattern == null || content == null) {
			 return false;
		 }	 
		 final int pat_end = pattern.length() - 1;
		 final int content_end =  content.length() - 1;
		 return quickMatch(pattern, content, pat_end, content_end);
	 }
	 public  static boolean quickMatch(final String pattern, final String content, final int pat_end,  final int content_end) {
		 // use i, j for shorter typing:)
		 int i = pat_end;
		 int j = content_end;		 
		 // both completed
		 if(j == -1 && i == -1)
			 return true;   
		 
		 if(j >=0 && i == -1)
			 return false;   
		 
		 while( i>=0 && j>=0 ) {			
			 if(pattern.charAt(i) != content.charAt(j)) {
				 if(pattern.charAt(i) != '*') {
					 return false;
				 }
				 else {
					 if(i==0) {
						 // start with one *
						 while(j>=0) {
							 if(content.charAt(j) == esc_str)
								 return false;
							 j--;
						 }
						 return true;
					 }
					 else {						
						 i--;						
						 if(pattern.charAt(i) != '*') { // just one *, not **
							 final char cur_char = pattern.charAt(i);
							 while(j>=0) {
								 if(cur_char == content.charAt(j)) {
									 boolean bMatch = quickMatch(pattern, content, i-1, j-1);
									 if(bMatch) {
										 return true;
									 }
								 }
								 if(content.charAt(j) == esc_str) {
									 return false;
								 }									
								 j--;								
							 }
							 if(j == -1) {
								 return false;
							 }
						 }
						 else {  // double **				
							 // handle more than 2 "*" continuously, more "*"s equals to 2 "*"
							 while(i>0) {
								 if(pattern.charAt(i-1) == '*') {
									 i--;
								 }
								 else {
									 break;
								 }
							 }

							 if(i==0) {
								 return true;
							 }
							 else {
								 i--;
								 final char cur_char = pattern.charAt(i);
								 while(j>=0) {
									 if(cur_char == content.charAt(j)) {
										 boolean bMatch = quickMatch(pattern, content, i-1, j-1);
										 if(bMatch) {
											 return true;
										 }
									 }
									 j--;
								 }
								 if(j==-1) {
									 return false;
								 }
							 }
						 }
					 }
				 }						
			 }
			 else {
				 i--;
				 j--;
			 }
		 }
		 // both completed
		 if(j == -1 && i == -1)
			 return true;   
		 if(j >=0 && i == -1)
			 return false; 
		 while(i>=0 && pattern.charAt(i) == '*') {				
			 i--;
		 }				 
		 if(i == -1) {
			 return true;
		 }		 

		 return false;
	 }

	 
	 
	 
	 public static  boolean fastMatchPattern(final String pattern, final String content) {		
		 if (pattern == null || content == null) {
			 return false;
		 }
		 int pattern_end = pattern.length() - 1;	 
		 int content_end = content.length() - 1;		 
		 while(pattern_end>=0 && content_end>=0) {
			 if(pattern.charAt(pattern_end) == content.charAt(content_end)) {
				 pattern_end--;
				 content_end--;
			 }
			 else if(pattern.charAt(pattern_end ) == '*') {
				 break;
			 }
			 else {
				 return false;
			 }
		 }
		 if(pattern_end == -1 && content_end == -1)
			 return true;
		 
		 int str_start = 0;
		 int min_end = Math.min(pattern_end, content_end);
		 while(str_start <= min_end) {
			 if(pattern.charAt(str_start) == content.charAt(str_start)) {
				 str_start++;				 
			 }
			 else if(pattern.charAt(str_start ) == '*') {
				 break;
			 }
			 else {
				 return false;
			 }
		 }
		 if(str_start+1==pattern_end) {
			 return true;
		 }
		 
		 return fastMatch(pattern, content, pattern_end, content_end, str_start);
	 }
	 public  static boolean fastMatch(final String pattern, final String content,  int pat_end,  int content_end, final int str_start) {

		 // use i, j for shorter typing:)
		 int j = content_end;
		 int i = pat_end;
		 // both completed
		 if(j == str_start-1 && i == str_start-1)
			 return true;
		 
		 if(i == str_start-1 && j>=str_start)
			 return false;

		 while( i>=str_start && j>=str_start ) {			
			 if(pattern.charAt(i) != content.charAt(j)) {
				 if(pattern.charAt(i) != '*') {
					 return false;
				 }
				 else {
					 if(i==str_start) {
						 // start with one *
						 while(j>=str_start) {
							 if(content.charAt(j) == esc_str)
								 return false;
							 j--;
						 }
						 return true;
					 }
					 else {
						 // just one *, not **
						 i--;
						 final char cur_char = pattern.charAt(i);
						 if(cur_char != '*') {
							 
							 while(j>=str_start) {
								 if(cur_char == content.charAt(j)) {
									 boolean bMatch = fastMatch(pattern, content, i-1, j-1,str_start);
									 if(bMatch) {
										 return true;
									 }
								 }

								 if(content.charAt(j) == esc_str) {
									 return false;
								 }									
								 j--;								
							 }
							 if(j == str_start-1) {
								 return false;
							 }

						 }
						 else {  // double **						

							 // handle more than 2 "*" continuously, more "*"s equals to 2 "*"
							 while(i>str_start) {
								 if(pattern.charAt(i-1) == '*') {
									 i--;
								 }
								 else {
									 break;
								 }
							 }

							 if(i==str_start) {
								 return true;
							 }
							 else {
								 i--;
								 final char tmp_char = pattern.charAt(i);
								 while(j>=str_start) {
									 if(tmp_char == content.charAt(j)) {
										 boolean bMatch = fastMatch(pattern, content, i-1, j-1,str_start);
										 if(bMatch) {
											 return true;											 
										 }
									 }
									 j--;
								 }
								 if(j==str_start-1) {
									 return false;
								 }
							 }
						 }
					 }

				 }							

			 }
			 else {
				 i--;
				 j--;
			 }
		 }
		 // both completed
		 if(j == str_start-1 && i == str_start-1)
			 return true;

		 if(j==str_start-1 && i>=str_start ) {
			 while(i>=str_start) {
				 if(pattern.charAt(i) == '*') {
					 i--;
				 }
				 else {
					 return false;
				 }
			 }
			 if(i == str_start-1) {
				 return true;
			 }
		 }

		 return false;
	 }

	 
	 public static boolean quickMatchPatternChar(final char[] pattern_char, final char[] content_char) {		
		 int pattern_end = pattern_char.length - 1;	 
		 int content_end = content_char.length - 1;

		 
		 return quickMatchChar(pattern_char, content_char, pattern_end, content_end);
	 }
	 public static boolean quickMatchChar(final char[] pattern, final char[] content, int pat_end, int content_end) {
		 
		int j = content_end;
		int i = pat_end;
		 if(i == -1 && j>=0)
			 return false;
		 if(j==-1 && i>0 )
			 return false;
		 
		
		while( i>=0 && j>=0 ) {			
			if(pattern[i] != content[j]) {
				if(pattern[i] != '*') {
					return false;
				}
				else {
					if(i==0) {
						// start with one *
						while(j>=0) {
							if(content[j] == esc_str)
								return false;
							j--;
						}
						return true;
					}
					else {
						// just one *, not **
						if(pattern[i-1] != '*') {
							
							char next_char = pattern[i-1];
							while(j>=0) {
								if(next_char == content[j]) {
									boolean bMatch = quickMatchChar(pattern, content, i-2, j-1);
									if(bMatch) {
										return true;
									}
								}
								
								if(content[j] == esc_str) {
									return false;
								}									
								j--;								
							}
							if(j == -1) {
								return false;
							}
							
						}
						else {  // double **
							i--;
							
							// handle more than 2 "*" continuously 
							while(i>0) {
								if(pattern[i-1] == '*') {
									i--;
								}
								else {
									break;
								}
							}
							
							if(i==0) {
								return true;
							}
							else {
								char next_char = pattern[i-1];
								while(j>=0) {
									if(next_char == content[j]) {
										boolean bMatch = quickMatchChar(pattern, content, i-2, j-1);
										if(bMatch) {
											return true;
										}
									}
																					
									j--;
								}
								if(j==-1) {
									return false;
								}
							}
						}
					}
					
				}							
				
			}
			else {
				i--;
				j--;
			}
		}
		// both completed
		if(j == -1 && i == -1)
			return true;
		
		if(j==-1 && i>=0 ) {

			while(i>=0) {
				if(pattern[i] == '*') {
					i--;
				}
				else {
					return false;
				}
			}
			if(i == -1) {
				return true;
			}

		}
				 
		 return false;
	 }
	 

	 
	 public static boolean fastMatchPatternChar(final char[] pattern_char, final char[] content_char) {		
		 int pattern_end = pattern_char.length - 1;	 
		 int content_end = content_char.length - 1;

		 while(pattern_end>=0 && content_end>=0) {
			 if(pattern_char[pattern_end] == content_char[content_end]) {
				 pattern_end--;
				 content_end--;
			 }
			 else if(pattern_char[pattern_end ] == '*') {
				 break;
			 }
			 else {
				 return false;
			 }
		 }
		 if(pattern_end == -1 && content_end == -1)
			 return true;
		 
		 int str_start = 0;
		 int min_end = Math.min(pattern_end, content_end);
		 while(str_start <= min_end) {
			 if(pattern_char[str_start] == content_char[str_start]) {
				 str_start++;				 
			 }
			 else if(pattern_char[str_start ] == '*') {
				 break;
			 }
			 else {
				 return false;
			 }
		 }
		 if(str_start+1==pattern_end) {
			 return true;
		 }
		 return fastMatchChar(pattern_char, content_char, pattern_end, content_end, str_start);
	 }
	 public static boolean fastMatchChar(final char[] pattern, final char[] content, int pat_end, int content_end, final int str_start) {
		 
		int j = content_end;
		int i = pat_end;
		 if(i == str_start-1 && j>=str_start)
			 return false;
		 if(j==str_start-1 && i>str_start )
			 return false;
		 
		
		while( i>=str_start && j>=str_start ) {			
			if(pattern[i] != content[j]) {
				if(pattern[i] != '*') {
					return false;
				}
				else {
					if( i== str_start) {
						// start with one *
						while(j>=str_start) {
							if(content[j] == esc_str)
								return false;
							j--;
						}
						return true;
					}
					else {
						// just one *, not **
						if(pattern[i-1] != '*') {
							
							char next_char = pattern[i-1];
							while(j>= str_start) {
								if(next_char == content[j]) {
									boolean bMatch = fastMatchChar(pattern, content, i-2, j-1, str_start);
									if(bMatch) {
										return true;
									}
								}
								
								if(content[j] == esc_str) {
									return false;
								}									
								j--;								
							}
							if(j == str_start-1) {
								return false;
							}
							
						}
						else {  // double **
							i--;
							
							// handle more than 2 "*" continuously 
							while(i>str_start) {
								if(pattern[i-1] == '*') {
									i--;
								}
								else {
									break;
								}
							}
							
							if(i==str_start) {
								return true;
							}
							else {
								char next_char = pattern[i-1];
								while(j>=str_start) {
									if(next_char == content[j]) {
										boolean bMatch = fastMatchChar(pattern, content, i-2, j-1, str_start);
										if(bMatch) {
											return true;
										}
									}
																					
									j--;
								}
								if(j==str_start-1) {
									return false;
								}
							}
						}
					}
					
				}							
				
			}
			else {
				i--;
				j--;
			}
		}
		// both completed
		if(j == str_start-1 && i == str_start-1)
			return true;
		
		if(j==str_start-1 && i>=str_start ) {

			while(i>=str_start) {
				if(pattern[i] == '*') {
					i--;
				}
				else {
					return false;
				}
			}
			if(i == str_start-1) {
				return true;
			}

		}
				 
		 return false;
	 }
	 
//	 static boolean dp[][] = new boolean[128][128];
//
//	 public  static boolean quickMatchDP(final String pattern, final String content) {
//		 
//		 final int pLen = pattern.length();
//		 final int cLen = content.length();
//		 
//		 dp[pLen][cLen] = true;
//		 
//		 for(int k=pLen-1;k>=0; k--) {
//			 if(dp[k+1][cLen] == true && pattern.charAt(k) == '*' ) {
//				 dp[k][cLen] = true;
//			 }
//			 else {
//				 dp[k][cLen] = false;
//			 }
//				 
//		 }
//		 
//		 
//		 int i = pLen -1;
//		 int j = cLen -1;		 
//		 
//		 for(;i>=0; i--) {
//			 boolean isContinue = false;
//			 final char cur_char = pattern.charAt(i);
//			 j = cLen-1;
//			 
//			 if(cur_char != '*') {
//				 while(j>=0) {
//					 if(dp[i+1][j+1] && cur_char == content.charAt(j) ) {
//						 dp[i][j] = true;
//						 isContinue = true;
//					 }
//					 else {
//						 dp[i][j] = false;
//					 }
//					 j--;
//				 }
//			 }
//			 else {
//				 if(i==0 || pattern.charAt(i-1) != '*') { // last single *
//					 while(j>=0) {
//						 if((dp[i][j+1] == true && content.charAt(j) != esc_str) ||dp[i+1][j]==true) {
//							 dp[i][j] = true;
//							 isContinue = true;
//						 }
//						 else {
//							 dp[i][j] = false;
//						 }
//						 j--;
//					 }
//				 }
//				 else { // double **
//					 
//					 i--;
//					 int countStar = 2;
//					 while(i>0) {
//						 if(pattern.charAt(i-1) == '*') {
//							 i--;
//							 countStar++;
//						 }
//						 else {
//							 break;
//						 }
//					 }
//				     if(i==0) { // ** at the end, quick return;
//				    	 return true;
//				     }else {
//						 while(j>=0) {
//							 if(dp[i][j+1] == true||dp[i+countStar][j]==true ) {
//								 dp[i][j] = true;
//								 isContinue = true;
//							 }
//							 else {
//								 dp[i][j] = false;
//							 }
//							 j--;
//						 }
//				     }
//					 
//				 }
//
//			 }
//			 
//			 
//			 if(!isContinue) {
//				 return false;
//			 }
//		 }		 
//		 
//		 return dp[0][0];
//	 }
	 
	 
	 public  static boolean quickMatchFinal(final String pattern, final String content) {
         int pattern_end = pattern.length()-1;
         int content_end = content.length()-1;
     
         // get rid of the chars after and before the '*'s
         while(pattern_end>=0 && content_end>=0) {
			 if(pattern.charAt(pattern_end) == content.charAt(content_end)) {
				 pattern_end--;
				 content_end--;
			 }
			 else if(pattern.charAt(pattern_end ) == '*') {
				 break;
			 }
			 else {
				 return false;
			 }
		 }
		 if(pattern_end == -1 && content_end == -1)
			 return true;
		 
		 int pat_start = 0;
		 final int min_end = Math.min(pattern_end, content_end);
		 while(pat_start <= min_end) {
			 if(pattern.charAt(pat_start) == content.charAt(pat_start)) {
				 pat_start++;				 
			 }
			 else if(pattern.charAt(pat_start ) == '*') {
				 break;
			 }
			 else {
				 return false;
			 }
		 }
		 if(pat_start+1==pattern_end) {
			 return true;
		 }
		 
		 
		 int i=pattern_end;
		 int j=content_end;
       
      
         int last2Ai = pat_start-1;
         int last2Aj = pat_start-1;
         int type = 0; // 0=common; 1=*; 2=**

         do {
           
        	 if(type != 0) {
	   	        	 
	        	 if(last2Ai!=pat_start-1 ) {
	        		 i=last2Ai;
	            	 j=last2Aj;	 
	        	 }
        	 }        	      


        	 while(i>=pat_start && j>=pat_start) {
        		 final char cur_char = pattern.charAt(i);
        		 if(cur_char != content.charAt(j)) {        	
        			 if(cur_char != '*') {
        				 break;
        			 }
        			 else{
        				 if(i==pat_start ) {// last single *
        					 type= 1;
        					 while(j>=pat_start) {
        						 if(content.charAt(j) == esc_str) {
        							 break;
        						 }
        						 j--;
        					 }
        					 if(j==pat_start-1) {
        						 return true;
        					 }
        					 break;

        				 }
        				 else {
        					 i--;
        					 if(pattern.charAt(i) !='*') { // single *
        						 type=1;
        						 final char tmp_char = pattern.charAt(i);
        						 while(j>=pat_start) {
        							 if(tmp_char == content.charAt(j)) {
        								 int k=i-1;
        								 int l=j-1;
        								 while(k>=0 && l>=0) {
        									 if(pattern.charAt(k) == content.charAt(l)) {
        										 k--;
        										 l--;
        									 }
        									 else {
        										 break;
        									 }
        								 }
        								 if(pattern.charAt(k) == '*') {
        									 i = k;
        									 j = l;
        									 break;
        								 }
        							 }
        							 if(content.charAt(j) == esc_str) {
        								 break;
        							 }
        							 j--;								
        						 }
        						 if(j == pat_start-1) {        						
        							 return false;
        						 }
        						 if(content.charAt(j) == esc_str) {
    								 break;
    							 }
        						 continue;
        					 }
        					 else { // double **
        						 type = 2;
        						 // handle more than 2 "*" continuously, more "*"s equals to 2 "*"
        						 while(i>pat_start) {
        							 if(pattern.charAt(i-1) == '*') {
        								 i--;
        							 }
        							 else {
        								 break;
        							 }
        						 }

        						 if(i==pat_start) { // last double **
        							 return true;
        						 }
        						 else {
        							 i--;
        							 final char tmp_char = pattern.charAt(i);
        							 while(j>=pat_start) {
        								 if(tmp_char == content.charAt(j)) {
        									 int k=i-1;
        									 int l=j-1;
        									 while(k>=0 && l>=0) {
        										 if(pattern.charAt(k) == content.charAt(l)) {
        											 k--;
        											 l--;

        										 }
        										 else {
        											 break;
        										 }
        									 }
        									 if(pattern.charAt(k) == '*') {
        										 last2Ai = i+2;
        										 last2Aj = l-1;
        										 i = k;
        										 j = l;
        										 break;
        									 }

        								 }
        								 j--;
        							 }
        							 if(j==pat_start-1) {
        								 return false;
        							 }
        							 continue;
        						 }

        					 }
        				 }


        			 }
        		 }
        		 else {
        			 i--;
        			 j--;
        		 }
        	 }

                	 
        	 if(j==pat_start-1 && i>=pat_start) {
        		 while(i>=pat_start) {
        			 if(pattern.charAt(i)!='*') {
        				 break;
        			 }        				 
        		 }        		 
        	 }
        	 
        	 if(i==pat_start-1 && j==pat_start-1)
        		 return  true;

         }while(last2Ai!=pat_start-1);
         
         if(i==-1 && j==-1)
    		 return  true;
    	 
    	 if(j==-1 && i>=0) {
    		 while(i>=0) {
    			 if(pattern.charAt(i)!='*') {
    				 return false;
    			 }        				 
    		 }
    		 return true;
    	 }
         
		 return false;		
	 }
	 
	 static void char_case(String[] pattern_lines, String[] content_lines) {
		 int pattern_size = pattern_lines.length;
		 int content_size = content_lines.length;
		 
		 char[][] pattern_array = new char[pattern_size][];
		 
		 for(int i=0; i<pattern_size; i++) {
			pattern_array[i] = pattern_lines[i].toCharArray();
		 }
		 
		 char[][] content_array = new char[content_size][];
		 for(int i=0; i<content_size; i++) {
				content_array[i] = content_lines[i].toCharArray();
			 }
		 
		 long startTime = System.nanoTime();
		 for(int i=0; i<20; i++) {		 
		 for(char[] pattern_str:pattern_array) {		
//			 Pattern  pattern = convertGlobToRegex(new String(pattern_str));			
			 for(char[] content_str:content_array) {
//				 boolean bPattern = matchPattern(pattern, new String(content_str));
				 boolean bMatch = fastMatchPatternChar(pattern_str, content_str);
//				 if(bPattern!=bMatch) {
//					 System.out.println("!!!!! not match: " + pattern_str + " ; " + content_str);
//				 }
			 }
		 }
		 }
		 
		 long duration =  System.nanoTime() - startTime;
		
		System.out.println("Elapsed time: " +duration/1000+" us");
		System.out.println("Comparing done!");
		 
	 }
	 
	public static void main(String[] args) {
		

        boolean result = quickMatchPattern("*.SubFlow84558.**.signalList",
        		"Main.SubFlow84558.ScanSubFlow71907.suite22694.signalList" );

        System.out.println(result);
//       System.exit(0);
		String[] pattern_lines = readLines("pattern_list.csv");
		String[] content_lines = readLines("suite_list.csv");
		
		int content_length = content_lines.length;
		int[] content_lens = new int[content_length];
		
		for(int i=0; i<content_lines.length; i++) {
			content_lens[i] = content_lines[i].length()-1;
		}
		
		System.out.println("file loaded!");
//		
//		char_case(pattern_lines, content_lines);
//		System.exit(0);
//		
		
		int count = 0;
		long startTime = System.nanoTime();
	
		for(final String pattern_str:pattern_lines) {
//			Pattern  pattern = convertGlobToRegex(pattern_str);			
			for(String content_str:content_lines) {
				count++		;
//				boolean bPattern = matchPattern(pattern, content_str);
				boolean bMatch = quickMatchPattern(pattern_str, content_str);
//				boolean bCompare = stringCompare(pattern_str, content_str);
				
//				if(bPattern != bMatch) {
//					System.out.println("!!!!!!!!not match: " + pattern_str + " ; " + content_str +"; should be:"+ bPattern);
//					System.exit(0);
//				}
//				if(bMatch) {
//					System.out.println("match: " + pattern_str + " ; " + content_str);				 
//				}
//				count++;
//				if(count>1000000)
//					break;
			}
		}
		
		System.out.println(count);
		
		long duration =  System.nanoTime() - startTime;
		
		System.out.println("Elapsed time: " +duration/1000+" us");
		System.out.println("Comparing done!");

	}

}