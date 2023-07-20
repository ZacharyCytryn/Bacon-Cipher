#include "bacon.h"


int two_pow(int pow);


int two_pow(int pow) {
   int answer = 1;
   for (int i = 0; i < pow; i++) {
       answer *= 2;
   }
   return answer;
}


int encrypt(const char *plaintext, char *ciphertext) {
  // Whitespaces, numbers, and letters are not included in this length
  int cipher_text_alpha_length = 0;
  for (int i = 0; i < strlen(ciphertext); i++) {
      if (isalpha(ciphertext[i])) {
          cipher_text_alpha_length++;
      }
  }
  /* Each bacon code is length 6. 1 is subtracted from character_coded for EOM marker.
     If it is -1, there was insufficient space. */
  int potential_characters_coded = (cipher_text_alpha_length / 6) - 1;
  int bacon_value = 0;
  int bin_value = 0;
  int cipher_counter = 0;
  int non_alpha_value = 0;
  int decider = 0;
  if (potential_characters_coded > strlen(plaintext)) {
      decider = strlen(plaintext);
  }
  else {
      decider = potential_characters_coded;
  }
  if (potential_characters_coded != -1) {
      for (int i = 0; i < decider; i++) {
          if (isalpha(plaintext[i])) {
              bacon_value = toupper(plaintext[i]) - 65;
          }
          else {
              if (isdigit(plaintext[i]) || (ispunct(plaintext[i]) && (plaintext[i] == ',' || plaintext[i] == '-' || plaintext[i] == '.' || plaintext[i] == '/'))) {
                  bacon_value = plaintext[i] - 8;
              }
              else {
                  if (isspace(plaintext[i]) || (ispunct(plaintext[i]) && plaintext[i] != '?')) {
                      bacon_value = plaintext[i] - 6;
                  }
                  else {
                      if (plaintext[i] == '?') {
                          bacon_value = plaintext[i] - 11;
                      }
                  }
              }
          }
          for (int j = 5; j >= 0; j--) {
              bin_value = (bacon_value >> j) & 0b1;
              while (!isalpha(ciphertext[cipher_counter])) {
                      cipher_counter++;
              }
              if (bin_value != 0) {
                      ciphertext[cipher_counter] = toupper(ciphertext[cipher_counter]);
                      cipher_counter++;
              }
              else {
                      ciphertext[cipher_counter] = tolower(ciphertext[cipher_counter]);
                      cipher_counter++;
              }
          }
      }
      for (int x = 0; x < 6; x++) {
          while(!isalpha(ciphertext[cipher_counter])) {
              cipher_counter++;
          }
          ciphertext[cipher_counter] = toupper(ciphertext[cipher_counter]);
          cipher_counter++;
      }
  }




  if (strlen(plaintext) > potential_characters_coded) {
      return potential_characters_coded;
  }
  else {
      return strlen(plaintext);
  }
}




int decrypt(const char *ciphertext, char *plaintext) {
   if (strlen(plaintext) == 0) {
       return -1;
   }
    /*
   copy ciphertext array into new array. loop through:
   if isalpha, add to array as 1 or 0.
    Have EOM counter: if 1 is repeated 6 times in a row, break for loop.
    if 1 is not repeated, return error number (-3?)
   */

  // Counting all letters in cipher text (skipping spaces, symbols, numbers, etc)
  int alpha_count = 0;
  for (int i = 0; i < strlen(ciphertext); i++) {
    if (isalpha(ciphertext[i])) {
        alpha_count++;
    }
  }
  // Initializing array
  int bacon_code[alpha_count];
  for (int i = 0; i < alpha_count; i++) {
    bacon_code[i] = 0;
  }

  // If uppercase, put 1 in array. Otherwise, put 0. eom_flag is first index of first 1 in EOM
  int bacon_code_iterator = 0;
  int eom_counter = 0;
  int eom_flag = -1;
  for (int i = 0; i < strlen(ciphertext); i++) {
    if (isalpha(ciphertext[i])) {
        if (isupper(ciphertext[i])) {
            bacon_code[bacon_code_iterator] = 1;
            bacon_code_iterator++;
            eom_counter++;
            if (eom_counter == 6) {
                eom_flag = bacon_code_iterator - 5;
                break;
            }
        }
        else {
            bacon_code[bacon_code_iterator] = 0;
            bacon_code_iterator++;
            eom_counter = 0;
        }
    }
  }
  if (eom_flag < 0) {
    return -2;
  }
  
  int total_letters = (eom_flag - 1) / 6;

  // bacon_code is now full of bacon codes and EOM at the end starting at index eom_flag (presumably)
  // Now translate - use two if statements: One if plaintext is long enough and one if plaintext is too short.
  // If plaintext is long enough, EOM_Flag is upperbound (might need to add null terminator)
  int plaintext_iterator = 0;
  int bacon_sum = 0;
  int pow_fixer = 0;
  // Add else statement for if plaintext is longer than total letters (ensure to add = to one of them?)

  // START CHECKING FOR -3 ERROR CHECK

    for (int i = 0; i < (eom_flag - 1); i += 6) {
        pow_fixer = 5;
        for (int j = 0; j < 6; j++) {
            if (bacon_code[i+j] == 1) {
                bacon_sum += two_pow(pow_fixer);
            }
            pow_fixer--;
        }
        if (bacon_sum >= 53 && bacon_sum <= 62) {
            return -3;
        }
        bacon_sum = 0;
    }
    // END FOR -3 ERROR CHECK

  if (strlen(plaintext) >= total_letters) {
    for (int i = 0; i < (eom_flag - 1); i += 6) {
        pow_fixer = 5;
        for (int j = 0; j < 6; j++) {
            if (bacon_code[i+j] == 1) {
                bacon_sum += two_pow(pow_fixer);
            }
            pow_fixer--;
        }
        if (bacon_sum >= 53 && bacon_sum <= 62) {
            return -3;
        }
        else {
            if (bacon_sum == 52) {
                bacon_sum += 11;
            }
            else {
                if (bacon_sum >= 50) {
                    bacon_sum += 6;
                }
                else {
                    if (bacon_sum >= 36) {
                        bacon_sum += 8;
                    }
                    else {
                        if (bacon_sum >= 26) {
                            bacon_sum += 6;
                        }
                        else {
                            bacon_sum += 65;
                        }
                    }
                }
            }
        }
        plaintext[plaintext_iterator] = bacon_sum;
        plaintext_iterator++;
        bacon_sum = 0;
    }
    plaintext[plaintext_iterator] = 0;
  }
  else {
    for (int i = 0; i < (eom_flag - 1); i += 6) {
        if (plaintext_iterator != strlen(plaintext) - 1) {
            pow_fixer = 5;
            for (int j = 0; j < 6; j++) {
                if (bacon_code[i+j] == 1) {
                    bacon_sum += two_pow(pow_fixer);
                }
                pow_fixer--;
            }
            if (bacon_sum >= 53 && bacon_sum <= 62) {
                return -3;
            }
            else {
                if (bacon_sum == 52) {
                    bacon_sum += 11;
                }
                else {
                    if (bacon_sum >= 50) {
                        bacon_sum += 6;
                    }
                    else {
                        if (bacon_sum >= 36) {
                            bacon_sum += 8;
                        }
                        else {
                            if (bacon_sum >= 26) {
                                bacon_sum += 6;
                            }
                            else {
                                bacon_sum += 65;
                            }
                        }
                    }
                }
            }
            plaintext[plaintext_iterator] = bacon_sum;
            plaintext_iterator++;
            bacon_sum = 0;
        }
        else {
            break;
        }
    }
    plaintext[plaintext_iterator] = 0;
  }

  int fin_letters = 0;
  for (int i = 0; i < strlen(plaintext); i++) {
    fin_letters++;
  }
  return fin_letters;
}