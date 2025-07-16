from collections import defaultdict

# Function to load words from a file into a list
def load_file(file_path):
    with open(file_path, 'r') as file:
        return [line.strip() for line in file]

# Function to group words by length and save them into separate files
def group_words_by_length(words_file):
    # Load words from the file
    words = load_file(words_file)
    
    # Create a dictionary to group words by length
    length_groups = defaultdict(list)
    for word in words:
        length_groups[len(word)].append(word)
    
    # Write words of each length to its corresponding file
    for length, words_of_length in length_groups.items():
        output_file = f"words{length}.txt"
        with open(output_file, 'w') as file:
            for word in sorted(words_of_length):  # Optionally, sort the words alphabetically
                file.write(word + '\n')
        print(f"Created file: {output_file}")

# Main execution
if __name__ == "__main__":
    words_file = "words.txt"  # Path to the input words file
    group_words_by_length(words_file)

