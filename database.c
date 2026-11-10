
#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <mysql/mysql.h>

static MYSQL *connection = NULL;

/* Get environment variable or use default value */
static const char *get_setting(const char *name, const char *fallback)
{
    const char *value = getenv(name);

    if (value == NULL || value[0] == '\0') {
        return fallback;
    }

    return value;
}

/* Connect to MySQL */
int db_connect(void)
{
    const char *host = get_setting("SNAKE_DB_HOST", "localhost");
    const char *user = get_setting("SNAKE_DB_USER", "root");
    const char *password = get_setting("SNAKE_DB_PASSWORD", "");
    const char *database = get_setting("SNAKE_DB_NAME", "snake_game");
    const char *port_value = get_setting("SNAKE_DB_PORT", "3306");

    char *end = NULL;
    unsigned long port = strtoul(port_value, &end, 10);

    if (end == port_value || *end != '\0' ||
        port == 0 || port > 65535) {
        fprintf(stderr, "Invalid MySQL port.\n");
        return 0;
    }

    if (connection != NULL) {
        return 1;
    }

    connection = mysql_init(NULL);

    if (connection == NULL) {
        fprintf(stderr, "MySQL initialization failed.\n");
        return 0;
    }

    if (mysql_real_connect(
            connection,
            host,
            user,
            password,
            database,
            (unsigned int)port,
            NULL,
            0) == NULL) {

        fprintf(stderr, "MySQL connection failed: %s\n",
                mysql_error(connection));

        mysql_close(connection);
        connection = NULL;
        return 0;
    }

    printf("Connected to MySQL successfully.\n");
    return 1;
}

/* Register a new player or retrieve an existing player */
int db_get_or_create_player(const char *username)
{
    MYSQL_STMT *stmt;
    MYSQL_BIND param[1];
    my_ulonglong player_id;
    unsigned long username_length;

    const char *query =
        "INSERT INTO players (username) VALUES (?) "
        "ON DUPLICATE KEY UPDATE "
        "player_id = LAST_INSERT_ID(player_id)";

    if (connection == NULL || username == NULL) {
        return -1;
    }

    username_length = (unsigned long)strlen(username);

    if (username_length == 0 ||
        username_length > DB_USERNAME_MAX) {
        fprintf(stderr, "Username must be between 1 and 50 characters.\n");
        return -1;
    }

    stmt = mysql_stmt_init(connection);

    if (stmt == NULL) {
        return -1;
    }

    if (mysql_stmt_prepare(stmt, query,
                           (unsigned long)strlen(query)) != 0) {
        fprintf(stderr, "Player query preparation failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = (void *)username;
    param[0].buffer_length = username_length;
    param[0].length = &username_length;

    if (mysql_stmt_bind_param(stmt, param) != 0 ||
        mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "Player registration failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    player_id = mysql_stmt_insert_id(stmt);
    mysql_stmt_close(stmt);

    if (player_id == 0 || player_id > INT_MAX) {
        return -1;
    }

    printf("Player account loaded successfully.\n");
    return (int)player_id;
}

/* Save a completed game and update player statistics */
int db_save_game_result(int player_id, int score)
{
    MYSQL_STMT *stmt;
    MYSQL_BIND params[2];

    const char *history_query =
        "INSERT INTO game_history (player_id, score) VALUES (?, ?)";

    const char *update_query =
        "UPDATE players SET "
        "high_score = GREATEST(high_score, ?), "
        "games_played = games_played + 1 "
        "WHERE player_id = ?";

    int update_score = score;
    int update_player = player_id;

    if (connection == NULL || player_id <= 0 || score < 0) {
        return 0;
    }

    if (mysql_query(connection, "START TRANSACTION") != 0) {
        return 0;
    }

    /* Insert score into game history */
    stmt = mysql_stmt_init(connection);

    if (stmt == NULL) {
        goto rollback;
    }

    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = &player_id;

    params[1].buffer_type = MYSQL_TYPE_LONG;
    params[1].buffer = &score;

    if (mysql_stmt_prepare(stmt, history_query,
                           (unsigned long)strlen(history_query)) != 0 ||
        mysql_stmt_bind_param(stmt, params) != 0 ||
        mysql_stmt_execute(stmt) != 0) {

        fprintf(stderr, "Score history error: %s\n",
                mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        goto rollback;
    }

    mysql_stmt_close(stmt);

    /* Update high score and games played */
    stmt = mysql_stmt_init(connection);

    if (stmt == NULL) {
        goto rollback;
    }

    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = &update_score;

    params[1].buffer_type = MYSQL_TYPE_LONG;
    params[1].buffer = &update_player;

    if (mysql_stmt_prepare(stmt, update_query,
                           (unsigned long)strlen(update_query)) != 0 ||
        mysql_stmt_bind_param(stmt, params) != 0 ||
        mysql_stmt_execute(stmt) != 0 ||
        mysql_stmt_affected_rows(stmt) == 0) {

        fprintf(stderr, "Player statistics update failed: %s\n",
                mysql_stmt_error(stmt));

        mysql_stmt_close(stmt);
        goto rollback;
    }

    mysql_stmt_close(stmt);

    if (mysql_query(connection, "COMMIT") != 0) {
        goto rollback;
    }

    printf("Game score saved successfully.\n");
    return 1;

rollback:
    mysql_query(connection, "ROLLBACK");
    return 0;
}

/* Retrieve the global top 10 leaderboard */
int db_get_leaderboard(
    LeaderboardEntry entries[DB_LEADERBOARD_LIMIT],
    int *count)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    int index = 0;

    const char *query =
        "SELECT username, high_score, games_played "
        "FROM players "
        "ORDER BY high_score DESC, username ASC "
        "LIMIT 10";

    if (count != NULL) {
        *count = 0;
    }

    if (connection == NULL || entries == NULL || count == NULL) {
        return 0;
    }

    if (mysql_query(connection, query) != 0) {
        fprintf(stderr, "Leaderboard query failed: %s\n",
                mysql_error(connection));
        return 0;
    }

    result = mysql_store_result(connection);

    if (result == NULL) {
        return 0;
    }

    while ((row = mysql_fetch_row(result)) != NULL &&
           index < DB_LEADERBOARD_LIMIT) {

        snprintf(entries[index].username,
                 DB_USERNAME_MAX + 1,
                 "%s",
                 row[0] != NULL ? row[0] : "");

        entries[index].high_score =
            row[1] != NULL ? atoi(row[1]) : 0;

        entries[index].games_played =
            row[2] != NULL ? atoi(row[2]) : 0;

        index++;
    }

    mysql_free_result(result);

    *count = index;
    return 1;
}

/* Close MySQL connection */
void db_close(void)
{
    if (connection != NULL) {
        mysql_close(connection);
        connection = NULL;
    }
}
