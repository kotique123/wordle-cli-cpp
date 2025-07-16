# Function to load file content into a set for efficient lookup
def load_file(file_path):
    with open(file_path, 'r') as file:
        return set(line.strip() for line in file)

# Function to filter out names from words
def filter_names(words_file, names_file, output_file):
    # Load words and names
    words = load_file(words_file)
    names = load_file(names_file)
    
    # Remove names from the words set
    filtered_words = words - names
    
    # Write the filtered words to the output file
    with open(output_file, 'w') as file:
        for word in sorted(filtered_words):  # Optional: Sort the words alphabetically
            file.write(word + '\n')

# Main execution
if __name__ == "__main__":
    words_file = "words.txt"  # Path to the words file
    names_file = "names.txt"  # Path to the names file
    output_file = "filtered_words.txt"  # Path to the output file
    
    filter_names(words_file, names_file, output_file)
    print(f"Filtered words have been saved to {output_file}")

