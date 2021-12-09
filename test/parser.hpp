#include <vector>
#include <unordered_map>
#include <string>

std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ante metus dictum at tempor commodo ullamcorper. Neque egestas congue quisque egestas diam. Enim lobortis scelerisque fermentum dui faucibus in ornare quam. Phasellus vestibulum lorem sed risus ultricies tristique nulla aliquet. Posuere urna nec tincidunt praesent semper feugiat nibh sed. Faucibus nisl tincidunt eget nullam non nisi. Nunc sed velit dignissim sodales ut eu. Dictum non consectetur a erat nam. Ornare arcu dui vivamus arcu felis bibendum. Feugiat pretium nibh ipsum consequat nisl vel pretium. Dis parturient montes nascetur ridiculus mus mauris vitae ultricies leo. Cursus euismod quis viverra nibh. Ac feugiat sed lectus vestibulum mattis ullamcorper. Libero enim sed faucibus turpis in eu mi bibendum neque.";

typedef struct {
    char *chars;
    int *freqs;
    int num_recorded;
} parse_package ;

static parse_package parse_text(void)
{
    /*
     * TOP
     * 
     * Parse the global lorem ipsum "text" and return
     * the statistics about it
     */

    /*
     * Parse
     */ 
    std::unordered_map<char, int> char_to_freq;
    for (char c : text) 
        char_to_freq[c]++;

    
    /*
     * Create package
     */
    parse_package package;
    package.chars = (char *) malloc(sizeof(char) * char_to_freq.size());
    package.freqs = (int *) malloc(sizeof(int) * char_to_freq.size());
    package.num_recorded = char_to_freq.size();
    int idx = 0;
    for (auto const &[c, f] : char_to_freq)
    {
        package.chars[idx] = c;
        package.freqs[idx] = f;
        idx++;
    }
    

    return package;
}