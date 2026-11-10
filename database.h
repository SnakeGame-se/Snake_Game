
#ifndef DATABASE_H
#define DATABASE_H

#define DB_USERNAME_MAX 50
#define DB_LEADERBOARD_LIMIT 10

typedef struct {
    char username[DB_USERNAME_MAX + 1];
    int high_score;
    int games_played;
} LeaderboardEntry;

/* Connect to the MySQL database. */
int db_connect(void);

/* Find an existing player or create a new one. */
int db_get_or_create_player(const char *username);

/* Save a completed game's score. */
int db_save_game_result(int player_id, int score);

/* Retrieve the global top 10 leaderboard. */
int db_get_leaderboard(
    LeaderboardEntry entries[DB_LEADERBOARD_LIMIT],
    int *count
);

/* Close the database connection. */
void db_close(void);

#endif
