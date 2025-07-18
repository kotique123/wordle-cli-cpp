# Function to load file content into a list
def load_file(file_path):
    with open(file_path, 'r') as file:
        return [line.strip() for line in file]

# Function to filter out words shorter than 5 characters
def filter_short_words(words_file, output_file):
    # Load words from the file
    words = load_file(words_file)
    
    # Filter words that are 5 characters or longer
    filtered_words = [word for word in words if len(word) >= 5]
    
    # Write the filtered words to the output file
    with open(output_file, 'w') as file:
        for word in filtered_words:
            file.write(word + '\n')

# Main execution
if __name__ == "__main__":
    words_file = "words.txt"  # Path to the words file
    output_file = "filtered_words_5chars.txt"  # Path to the output file
    
    filter_short_words(words_file, output_file)
    print(f"Filtered words have been saved to {output_file}")

