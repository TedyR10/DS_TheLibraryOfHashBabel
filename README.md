**Name: Rolea Theodor-Ioan**
**Group: 313CA**

## The Library of HashBabel - HW 2

### Description:
* The program employs straightforward data structures, namely a linked list and hashtable. The linked list functionalities encompass list creation, node addition, and removal, while the hashtable leverages the linked list structure. Hashtable operations encompass the creation, deletion, key-value pairing, key existence checking, and value retrieval. Initially, the hashtable features a predefined number of buckets, denoted as HMAX. However, it dynamically adjusts its size if the entries surpass the number of buckets. When adding an entry with an already-existing key, the prior associated value is overwritten. Memory deallocation is facilitated through a designated "free_function," with a corresponding pointer saved within the hashtable structure, alongside hashing and comparison function pointers.

* The program is designed to implement both a library and a user database using hashtables. Within this system, a book within the library is also represented as a hashtable, with each book containing various definitions, each composed of a key and value pair. Using the library commands, users can perform actions like adding a book, retrieving book information, removing a book, adding definitions to a book, retrieving and printing definitions, and deleting definitions.

* For the user database (Users), actions include adding users, borrowing and returning books, and reporting lost books. Each user is assigned a score, initially set to 100, and this score can increase or decrease based on book return times and loss incidents. If a user's score turns negative, they are banned from the library, which results in their removal from the database hashtable and addition to a separate list of banned usernames.

* The program also offers a set of commands, including:

- ADD_BOOK: Creates a book structure, accumulates book-related information, and adds it to the library.
- GET_BOOK: Retrieves and prints information about a specific book from the library.
- RMV_BOOK: Removes a book from the library hashtable.
- ADD_DEF: Adds a definition to a specific book in the library.
- GET_DEF: Retrieves and prints a definition from a book in the library.
- RMV_DEF: Removes a definition from a book in the library.
- ADD_USER: Establishes a user structure with relevant information and adds it to the database.
- BORROW: Marks a book as borrowed by a user, setting the time limit.
- RETURN: Updates book and user information when a book is returned, including user score calculation.
- LOST: Decreases a user's score and removes a book from the library when a book is reported as lost.
- EXIT: This command triggers the program to print all books sorted by average rating, borrowing frequency, and lexicographical order. It also prints all users sorted by score and lexicographical order before freeing all dynamically allocated memory.

* For hashing strings, the program uses the hashing function described at http://www.cse.yorku.ca/~oz/hash.html.
