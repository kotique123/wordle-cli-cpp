import sqlite3
import os

for i in range(5, 31):
    db_name = f'words{i}.db'
    txt_name = f'words{i}.txt'
    if not os.path.isfile(txt_name):
        continue  # Skip to next iteration if file does not exist
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()

    cursor.execute('''CREATE TABLE IF NOT EXISTS words
                    (id INTEGER PRIMARY KEY AUTOINCREMENT,
                    word TEXT UNIQUE)''')

    with open(txt_name, 'r') as f:
        for line in f:
            word = line.strip()  # Remove any whitespace/newlines
            if word:  # Only insert if word is not empty
                try:
                    cursor.execute("INSERT INTO words (word) VALUES (?)", (word,))
                except sqlite3.IntegrityError:
                    pass  # Skip duplicate words (due to UNIQUE constraint)

    conn.commit()
    conn.close()