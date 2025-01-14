#ifndef _INC_ERRORMESSAGES_H
#define _INC_ERRORMESSAGES_H

// Error messages
#define ERRMSG_PREFIX "[ERROR] "

#define ERRMSG_MALLOC_FAILED ERRMSG_PREFIX "Nie udało się zarezerwować pamięci"
#define ERRMSG_FILE_OPEN_FAILED(file) ERRMSG_PREFIX "Nie udało się otworzyć pliku \"" file "\""
#define ERRMSG_FILE_CLOSE_FAILED ERRMSG_PREFIX "Nie udało się zamknąć pliku"

// UTF-8 error messages
#define ERRMSG_UTF8_GENERIC ERRMSG_PREFIX "Niepoprawna sekwencja UTF-8"
#define ERRMSG_UTF8_CONTINUATION ERRMSG_UTF8_GENERIC " (Bajt jest kontynuacją, a nie początkiem)"
#define ERRMSG_UTF8_INVALID(c) ERRMSG_UTF8_GENERIC " (Niepoprawny bajt UTF-8: %c (%d))", c, c

// ANSI error messages
#define ERRMSG_ANSI_CURSOR_FAILED ERRMSG_PREFIX "Nie udało się pozyskać pozycji kursora, kody ANSI nie są wspierane?"
#define ERRMSG_ANSI_CHECK_FAILED ERRMSG_PREFIX "Kody ANSI nie są wspierane przez twój terminal, ta aplikacja nie może bez nich działać!"

// Question error messages
#define ERRMSG_QUESTION_PREFIX ERRMSG_PREFIX "%s:%d | "
#define ERRMSG_QUESTION_FAILED_TO_DESERIALIZE(file, line, question) ERRMSG_QUESTION_PREFIX "Nie udało się odczytać pytania \"%s\"\n", file, line, question
#define ERRMSG_QUESTION_INVALID_OUT_OF_RANGE_QUESTION_ID(file, line, id) ERRMSG_QUESTION_PREFIX "Niepoprawny identyfikator pytania: %d, identyfikator musi być dodatni (większy od 0)\n", file, line, id
#define ERRMSG_QUESTION_DUPLICATE_QUESTION_ID(file, line, id) ERRMSG_QUESTION_PREFIX "Duplikat identyfikatora pytania: %d\n", file, line, id
#define ERRMSG_QUESTION_FAILED_TO_DESERIALIZE_ID(question) ERRMSG_PREFIX "Nie udało się odczytać identyfikatora pytania: \"%s\"\n", question
#define ERRMSG_QUESTION_FAILED_TO_DESERIALIZE_CONTENT(question) ERRMSG_PREFIX "Nie udało się odczytać treści pytania: \"%s\"\n", question
#define ERRMSG_QUESTION_FAILED_TO_DESERIALIZE_ANSWER(ansId, question) ERRMSG_PREFIX "Nie udało się odczytać odpowiedzi nr %d: \"%s\"\n", ansId, question
#define ERRMSG_QUESTION_INVALID_QUESTION_ID(question) ERRMSG_PREFIX "Niepoprawny identyfikator pytania: \"%s\"\n", question
#define ERRMSG_QUESTION_INVALID_QUESTION_ID_PERRNO ERRMSG_PREFIX "Niepoprawny identyfikator pytania"
#define ERRMSG_QUESTION_FAILED_TO_LOAD ERRMSG_PREFIX "Nie udało się załadować pytań"
#define ERRMSG_QUESTION_CONTENT_EMPTY ERRMSG_PREFIX "Treść pytania jest pusta"

// General error messages
#define ERRMSG_INVALID_OPTION_INDEX(index) ERRMSG_PREFIX "Niepoprawny indeks opcji: %d\n", index
#define ERRMSG_OPTION_OUT_OF_BOUNDS ERRMSG_PREFIX "Indeks opcji poza zakresem\n"
#define ERRMSG_OPTION_NOT_FOUND ERRMSG_PREFIX "Nie znaleziono opcji\n"
#define ERRMSG_NULL_TERMINATOR_MISSING ERRMSG_PREFIX "Brak znaku NULL w ciągu znaków\n"
#define ERRMSG_UNEXPEDTED_END_OF_STRING ERRMSG_PREFIX "Nieoczekiwany koniec bufora, nie znaleziono znaku NULL\n"
#define ERRMSG_INDEX_OUT_OF_RANGE ERRMSG_PREFIX "Indeks poza zakresem\n"

#define ERRMSG_ERROR_POPUP_TITLE "Błąd"
#define ERRMSG_TOO_SMALL_TERMINAL(x, y) "Terminal jest zbyt mały aby wyświetlić to okno. Minimalne wymagania to: %d x %d", x, y

// Validation failure messages
#define VLDFAIL_QUESTION_ID_INVALID "Niepoprawny identyfikator pytania, musi być dodatni"
#define VLDFAIL_QUESTION_CONTENT_EMPTY "Treść pytania jest pusta"
#define VLDFAIL_QUESTION_ANSWER_EMPTY "Odpowiedź jest pusta"
#define VLDFAIL_QUESTION_LIST_NULL "Błąd wewnętrzny: Lista pytań jest niezaładowana"
#define VLDFAIL_QUESTION_DUPLICATE_ID "Duplikat identyfikatora pytania"
#define VLDFAIL_QUESTION_NOT_FOUND "Nie znaleziono pytania"

#endif // _INC_ERRORMESSAGES_H