#include "StringUtils.h"

namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept{

    int n = str.length(); // get length of string
    int start_index, end_index;
    std::string result;

    //recalculate start index to make it non-negative if within bounds
    if (start >= 0){
        start_index = start; // if start is non-negative, count from beginning
    } 
    if (start < 0) {
        start_index = start + n; // if start is negative, count backwards n from the end 
    }

    
    //recalculate end index to make it non-negative
    if (end > 0) {
        end_index = end; // if end is positive, count from beginning
    }
    if (end < 0) {
        end_index = end + n; // if end is negative, count backwards n from the end
    }
    if (end == 0) {
        end_index = n; // if end is 0, slice to the end same as s[:n] in python
    }
    
    //check for out of bounds
    if (start_index >= n || end_index < 0) {
        return ""; //like "abc"[100:] or "abc"[-15:-12]
    }
    
    //check for start index greater than end index
    if (start_index >= end_index) {
        return ""; // like "abc"[2:1] or "abc"[-1:-2]
    }
    else{
        if (start_index < 0) {
            start_index = 0; // clamp start index to 0 if negative after recalculation
        }
        if (end_index > n) {
            end_index = n; // clamp end index to n if greater than n after recalculation
        }
        result = str.substr(start_index, end_index - start_index);
        return result;
    }

    
}

std::string Capitalize(const std::string &str) noexcept{

    std::string Temp = str;
    if (!Temp.empty()){
        Temp[0] = toupper(Temp[0]);//capitalize first character
        for(size_t Index = 1; Index < Temp.length(); Index++){
            Temp[Index] = tolower(Temp[Index]);//lowercase the rest
        }
    }
    return Temp;
    

}

std::string Upper(const std::string &str) noexcept{
    // Capitalize all characters in the string
    std::string Temp = str;
    if(!Temp.empty()){
        for(size_t Index = 0; Index < Temp.length(); Index++){
            Temp[Index] = toupper(Temp[Index]);
        }
        return Temp;
    }
    return "";
}

std::string Lower(const std::string &str) noexcept{
    // Lowercase all characters in the string
    std::string Temp = str;
    if(!Temp.empty()){
        for(size_t Index = 0; Index < Temp.length(); Index++){
            Temp[Index] = tolower(Temp[Index]);
        }
        return Temp;
    }
    return "";
}

std::string LStrip(const std::string &str) noexcept{
    // Replace code here
    std::string Temp = str;
    size_t Index = 0;
    size_t Length = str.length();
    while (Index < Length && isspace(str[Index])){
        Index++;
    }
    return Temp.substr(Index);
}

std::string RStrip(const std::string &str) noexcept{
    // Replace code here
    std::string Temp = str;
    if(!Temp.empty()){
        size_t Index = str.length() - 1;
        while(isspace(str[Index])){
            if (Index == 0){  //empty string after stripping
                return "";
            }
            Index--;
        }
        return Temp.substr(0, Index + 1);// +1 because to the last non-space character
    }    
    return "";
}

std::string Strip(const std::string &str) noexcept{
    return LStrip(RStrip(str));
}

std::string Center(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    size_t n = str.length();
    int length = static_cast<int>(str.length());
    if (width <= length) {
        return str; // No padding needed if width is less than or equal to string length
    }
    else{
        int total = width - length;
        int right = total / 2;
        int left = total - right;
        result.insert(0, left, fill); // Insert left padding
        result.append(right, fill);   // Append right padding
        return result;
    }
}

std::string LJust(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    size_t n = str.length();
    int length = static_cast<int>(str.length());
    // if we don't do this conversion, width <= n will automatically convert n to unsigned int 
    // this will cause "terminate called after throwing an instance of 'std::length_error'"

    if (width <= length) {
        return str; // No trailing padding needed if width is less than or equal to string length
    }
    else{
        result.append(width - length, fill); // Append (width - n) fill characters to the right
        return result;
    }
}

std::string RJust(const std::string &str, int width, char fill) noexcept{
    std::string result = str;
    size_t n = str.length();
    int length = static_cast<int>(str.length());
    if (width <= length) {
        return str; // No leading padding needed if width is less than or equal to string length
    }
    else{
        result.insert(0, width - length, fill); // Insert (width - n) fill characters to the left
        return result;
    }
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept{
    std::string result= str;
    size_t pos = 0; // current position in the original string
    size_t old_len = old.length();
    size_t rep_len = rep.length();
    // if old string is empty, we insert rep between every character including the beginning and end
    if (old.empty()) {        
        size_t n = str.length();
        result = "";
        for (size_t i = 0; i < n; i++){
            result += rep + str[i]; // add rep before each character
        }
        result += rep; // append rep at the end
        return result;
    }   
    else{
        while(pos + old_len <= str.length()){
            pos = result.find(old,pos); //update pos to the position of found old substring
            //if we find a match,str.find(old,pos)!= std::string::npos, we replace it
            if (pos != std::string::npos){    
                result = result.substr(0,pos) + rep + StringUtils::Slice(result, pos + old_len);
                //replace old with rep, taking advantage of Slice function when slicing to the end
                pos += rep_len; //move pos forward by rep_len to continue searching after the replaced substring
            }
            else{
                break; //no match anymore, skip the loop
            }
        }
        return result;
    }
}

std::vector< std::string > Split(const std::string &str, const std::string &splt) noexcept{
    //My method is to use string::find to locate the delimiter and use the Slice that I've implemented to extract substrings
    
    std::vector< std::string > result;
    size_t n = str.length();
    size_t splt_len = splt.length();

    if (n == 0){
        //original string is empty
        if (splt.empty()){
            //if splt is also empty, return empty vector
            return result;
        }
        else{
            //if splt is not empty, return vector with one element: empty string
            result.push_back("");
            return result;
        }
    }  
    // original string is not empty
    else{
         // if splt parameter is emptystring, then split on whitespace
        if(splt.empty()){
            int flag; //flag to indicate whether we are in a word or in whitespace 1: in word, 0: in whitespace
            //initialize flag
            if (n > 0 && !isspace(str[0])){
                flag = 1; //first character is not space, we are in a word
            }
            else{
                flag = 0; //first character is space or string is empty, we are in whitespace
            }
            size_t curr = 0; //current index
            size_t word_start = 0; //start index of a word
            for(size_t i = 0; i < n; i++){
                if (!isspace(str[i]) && flag == 1){
                    //we are in a word, continue
                    continue;
                }
                else if (!isspace(str[i]) && flag == 0){
                    //we encounter a non-space character after spaces, which is the start of a new word
                    word_start = i;
                    flag = 1;
                }
                else if (isspace(str[i]) && flag == 1){
                    //we encounter a space after being in a word, slice the word and add to result
                    result.push_back(StringUtils::Slice(str, word_start, i));
                    flag = 0;
                }
                else{
                    //we are in spaces, continue
                    continue;
                }
            }
            //after the loop, we may end while being in a word, need to add the last word
            if (flag == 1){
                result.push_back(StringUtils::Slice(str, word_start, n));
            }
        }
        // if splt is not emptystring, split on the specified delimiter
        else{
            std::string not_traversed = str; //This string keeps track of the part of the string that has not been processed
            size_t pos = str.find(splt); //position of first occurrence 
            // if no occurrence found, return vector with original string as the only element
            if (pos == std::string::npos){
                result.push_back(str);
                return result;  
            }
            else{
                while(pos != std::string::npos){
                    //if the first character is the delimiter, we need to add an empty string to the result
                    if(pos == 0){
                        result.push_back("");
                        not_traversed = StringUtils::Slice(not_traversed, pos + splt_len);
                        pos = not_traversed.find(splt);
                    }
                    else{
                        result.push_back(StringUtils::Slice(not_traversed, 0, pos));
                        // if there are consecutive delimiters, we need to add empty strings to the result                
                        not_traversed = StringUtils::Slice(not_traversed, pos + splt_len);
                        pos = not_traversed.find(splt);
                    }
                    
                }
                //after the loop, we may have some remaining part in not_traversed, add it to result
                result.push_back(not_traversed);
            }
        }
    }
   
    return result;
}

std::string Join(const std::string &str, const std::vector< std::string > &vect) noexcept{
    std::string result = "";
    size_t vect_size = vect.size();
    for(size_t i = 0; i < vect_size; i++){
        result += vect[i];
        if (i != vect_size - 1){
            result += str; //add delimiter between elements, but not after the last element
        }
    }
    return result;
}
    
    // Implement ExpandTabs function Note:
    // For the situation where tabsize is > 0, my initial idea is to use find to search for it first. 
    //If it doesn't exist, I will directly return the original string. 
    //If it does, I will use a while loop to continuously evaluate find. 
    //In the while loop, I will calculate the column index at this time, add a few blanks, and then update the col index. 
    //After the while is over, I will directly append all the remaining strings
    //However, I find a problem with this method when testing with strings with \n
    //Switch to a character by character traversal method to correctly handle newlines

std::string ExpandTabs(const std::string &str, int tabsize) noexcept{

    //if tabsize is less than or equal to 0, remove all tabs by replacing with empty string
    if(tabsize <= 0){
        return StringUtils::Replace(str, "\t", "");
    }
    else{
        //if tabsize is positive, replace each tab with appropriate number of spaces
        //the key is to calculate the number of spaces needed to reach the next tab stop
        //the formula should be tab_size - (current_column % tab_size)
        std::string result = "";
        size_t n = str.length();
        int current_column = 0; //track the current column position in the result(modified) string
        
        //character-wise traversal
        for(size_t i = 0; i < n; i++){
            //if we meet a \t, we need to calculate the number of spaces to add to reach the next tab stop
            if (str[i] == '\t'){
                int spaces_to_add = tabsize - (current_column % tabsize);
                result.append(spaces_to_add, ' '); //append spaces
                current_column += spaces_to_add; //update current_column number
            }
            else if (str[i] == '\n'){
                //newline character, switch to the next line and reload current_column to 0
                result += '\n';
                current_column = 0;
            }
            else{
                //normal character, just append it and increment current_column
                result += str[i];
                current_column++;
            }
            
        }
        return result;
    }

}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept{
    //We can solve this implementation accorrding to the definition of Levenshtein distance on wikipedia
    size_t left_size = left.length();
    size_t right_size = right.length();
    //case 1: if either string is empty, the distance is the length of the other string
    if(left_size == 0){
        return static_cast<int>(right_size);
    }
    else if (right_size == 0){
        return static_cast<int>(left_size);
    }
    //case 2: both strings are non-empty, use recurssive relation
    else{
        std::string left_str = left;
        std::string right_str = right;
        if (ignorecase){
            left_str = StringUtils::Lower(left);
            right_str = StringUtils::Lower(right);
        }
        if(left_str == right_str){
            return 0;
        }
        else{
            //if first characters are the same, the we can simply calculate the edit distance of remaining substrings
            // we can make a recursive call on the remaining substrings
            if(left_str[0] == right_str[0]){
                std::string new_left = StringUtils::Slice(left_str, 1);
                std::string new_right = StringUtils::Slice(right_str, 1);
                return EditDistance(new_left, new_right, ignorecase);
            }
            // if first characters are different, we make recursive call to the EditDistance function assuming this is the only 
            // place that needed to be modified and add 1 for insertion/deletion/replacement.
            else{
                int lev_taila_b = EditDistance(StringUtils::Slice(left_str, 1),right_str); // insertion
                int lev_a_tailb = EditDistance(left_str,StringUtils::Slice(right_str, 1)); // deletion
                int lev_taila_telb = EditDistance(StringUtils::Slice(left_str, 1),StringUtils::Slice(right_str, 1)); //replacement
                return 1 + std::min(std::min(lev_taila_b,lev_a_tailb) , lev_taila_telb);
            }
        }
    }
}
}

