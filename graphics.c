#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>

#define MAX_WORDS 100
#define MAX_WORD_LENGTH 100
#define EASY_ATTEMPTS 10
#define MEDIUM_ATTEMPTS 7
#define HARD_ATTEMPTS 5

typedef struct {
    char name[50];
    int score;
} Player;

char ch;
char highest_scorer[50] = "None";
int highest_score = 0;

void rule() {
    FILE *game_rules = fopen("rules.txt", "r");
    if (game_rules == NULL) {
        printf("Error: Could not open rules.txt\n");
        exit(1);
    }

    while ((ch = fgetc(game_rules)) != EOF) {
        printf("%c", ch);
    }
    fclose(game_rules);
}

void display_banner() {
    printf("\n");
    printf("***********************************************\n");
    printf("*             WORD GUESSING GAME             *\n");
    printf("*           Test Your Vocabulary!            *\n");
    printf("***********************************************\n");
    printf("\n");
}

void display_game_over() {
    printf("\n***********************************************\n");
    printf("*                  GAME OVER                 *\n");
    printf("***********************************************\n");
    printf("\n");
}

void display_congratulations() {
    printf("\n***********************************************\n");
    printf("*            CONGRATULATIONS!               *\n");
    printf("***********************************************\n");
    printf("\n");
}

int load_words(char *filename, char words[MAX_WORDS][MAX_WORD_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s.\n", filename);
        exit(1);
    }

    int count = 0;
    while (fgets(words[count], MAX_WORD_LENGTH, file)) {
        words[count][strcspn(words[count], "\n")] = '\0';
        count++;
        if (count >= MAX_WORDS) {
            break;
        }
    }
    fclose(file);

    if (count == 0) {
        printf("Error: No words found in %s.\n", filename);
        exit(1);
    }
    return count;
}

void display_word_with_blanks(char *word, char *masked_word) {
    for (int i = 0; word[i] != '\0'; i++) {
        masked_word[i] = '_';
    }
    masked_word[strlen(word)] = '\0';
}

int update_masked_word(char guess, char *word, char *masked_word) {
    int found = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        if (tolower(word[i]) == tolower(guess) && masked_word[i] == '_') {
            masked_word[i] = word[i];
            found = 1;
        }
    }
    return found;
}

void choose_level(int *attempts, char *word_file) {
    int choice;
    printf("\nChoose a difficulty level:\n");
    printf("1. Easy (10 attempts)\n");
    printf("2. Medium (7 attempts)\n");
    printf("3. Hard (5 attempts)\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("\nSome Known Fruits. Hint: First letter would be anything starting from A to W! Now try yourself!\n");
            *attempts = EASY_ATTEMPTS;
            strcpy(word_file, "easy_words.txt");
            break;
        case 2:
            printf("\nSome Known Countries: But no hints! Guess by yourself!\n");
            *attempts = MEDIUM_ATTEMPTS;
            strcpy(word_file, "medium_words.txt");
            break;
        case 3:
            printf("It can be any fruit, city, or animal! But Something that represents Bangladesh! Think deeply and choose carefully!\n");
            *attempts = HARD_ATTEMPTS;
            strcpy(word_file, "hard_words.txt");
            break;
        default:
            printf("Invalid choice! Defaulting to Easy.\n");
            *attempts = EASY_ATTEMPTS;
            strcpy(word_file, "easy_words.txt");
            break;
    }
}

void save_score(const char *player_name, int score) {
    FILE *file = fopen("scores.txt", "a");
    if (!file) {
        printf("Error: Unable to open scores.txt\n");
        exit(1);
    }
    fprintf(file, "%s %d\n", player_name, score);
    fclose(file);
}

int load_leaderboard(Player leaderboard[], int max_players) {
    FILE *file = fopen("scores.txt", "r");
    if (!file) {
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%s %d", leaderboard[count].name, &leaderboard[count].score) == 2) {
        count++;
        if (count >= max_players) break;
    }

    fclose(file);
    return count;
}

void display_leaderboard(Player leaderboard[], int player_count, const char *latest_player, int latest_score) {
    printf("\n***********************************************\n");
    printf("*                  LEADERBOARD               *\n");
    printf("***********************************************\n");


    for (int i = 0; i < player_count - 1; i++) {
        for (int j = i + 1; j < player_count; j++) {
            if (leaderboard[i].score < leaderboard[j].score) {
                Player temp = leaderboard[i];
                leaderboard[i] = leaderboard[j];
                leaderboard[j] = temp;
            }
        }
    }

    int position = -1;
    for (int i = 0; i < player_count; i++) {
        printf("%d. %s - %d points\n", i + 1, leaderboard[i].name, leaderboard[i].score);
        if (strcmp(leaderboard[i].name, latest_player) == 0 && leaderboard[i].score == latest_score) {
            position = i + 1;
        }
    }

    printf("\nYour Position: %d\n", position);
}

int main() {
    srand(time(0));
    system("color 09");

    display_banner();
    rule();

    char player_name[50];
    printf("\nEnter your name: ");
    scanf("%s", player_name);

    int total_points = 0;
    int levels_completed = 0;

    while (1) {
        int max_attempts;
        char word_file[50];
        choose_level(&max_attempts, word_file);

        char words[MAX_WORDS][MAX_WORD_LENGTH];
        int word_count = load_words(word_file, words);

        int attempts = max_attempts;
        int guessed_word_index = -1;
        levels_completed++;

        while (attempts > 0) {
            int word_guessed = 0;
            if (guessed_word_index == -1) {
                guessed_word_index = rand() % word_count;
            }

            char masked_word[MAX_WORD_LENGTH];
            display_word_with_blanks(words[guessed_word_index], masked_word);

            while (!word_guessed && attempts > 0) {
                printf("\nWord: ");
                for (int i = 0; masked_word[i] != '\0'; i++) {
                    printf("%c ", masked_word[i]);
                }
                printf("\nAttempts left: %d\n", attempts);
                printf("Enter a letter: ");

                char guess;
                scanf(" %c", &guess);

                if (!isalpha(guess)) {
                    printf("Please enter a valid letter.\n");
                    continue;
                }

                if (!update_masked_word(guess, words[guessed_word_index], masked_word)) {
                    attempts--;
                    printf("Incorrect guess!\n");
                } else {
                    printf("Correct guess!\n");
                }

                if (strcmp(words[guessed_word_index], masked_word) == 0) {
                    display_congratulations();
                    printf("You guessed the word: %s\n", words[guessed_word_index]);
                    word_guessed = 1;

                    int points = 10;
                    if (levels_completed == 2) {
                        points = 20;
                    } else if (levels_completed == 3) {
                        points = 30;
                    }

                    total_points += points;
                    printf("You have earned %d points!\n", points);
                    break;
                }
            }

            if (word_guessed) {
                break;
            }
        }

        if (attempts == 0) {
            display_game_over();
            printf("The correct word was: %s\n", words[guessed_word_index]);
        }

        if (levels_completed == 3) {
            printf("\nWow! You have completed 3 levels!\n");
            break;
        }

        printf("\nTotal points earned so far: %d\n", total_points);

        char continue_game;
        printf("\nDo you want to continue playing? (y/n): ");
        scanf(" %c", &continue_game);

        if (continue_game == 'n' || continue_game == 'N') {
            break;
        }
    }

    save_score(player_name, total_points);

    Player leaderboard[MAX_WORDS];
    int player_count = load_leaderboard(leaderboard, MAX_WORDS);
    display_leaderboard(leaderboard, player_count, player_name, total_points);

    printf("\nThank you for playing! Your final score is %d\n", total_points);
    return 0;
}
