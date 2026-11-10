
#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <mysql/mysql.h>

static MYSQL *connection = NULL;

/* Connect to the MySQL database */
int db_connect(void)
{
    const char *host = getenv("SNAKE_DB_HOST");
    const char *user = getenv("SNAKE_DB_USER");
    const char *password = getenv("SNAKE_DB_PASSWORD");
    const char *database = getenv("SNAKE_DB_NAME");
    const char *port_value = getenv("SNAKE_DB_PORT");

    unsigned int port = 3306;

    if (host == NULL) host = "localhost";
    if (user == NULL) user = "root";
    if (password == NULL) password = "";
    if (database == NULL) database = "snake_game";

    if (port_value != NULL && *port_value != '\0') {
        char *end = NULL;
        unsigned long parsed_port = strtoul(port_value, &end, 10);

        if (*end == '\0' && parsed_port > 0 && parsed_port <= 65535) {
            port = (unsigned int)parsed_port;
        } else {
            fprintf(stderr, "Invalid MySQL port.\n");
            return 0;
        }
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
            port,
            NULL,
            0
        ) == NULL) {

        fprintf(stderr, "MySQL connection failed: %s\n",
                mysql_error(connection));

        mysql_close(connection);
        connection = NULL;
        return 0;
    }

    printf("Connected to MySQL database successfully.\n");
    return 1;
}

/* Find an existing player or create a new player */
int db_get_or_create_player(const char *username)
{
    MYSQL_STMT *stmt;
    MYSQL_BIND param[1];
    my_ulonglong player_id;
    size_t username_length;

    if (connection == NULL || username == NULL) {
        return -1;
    }

    username_length = strlen(username);

    if (username_length == 0 || username_length > DB_USERNAME_MAX) {
        fprintf(stderr, "Username must contain 1 to 50 characters.\n");
        return -1;
    }

    stmt = mysql_stmt_init(connection);

    if (stmt == NULL) {
        return -1;
    }

    const char *query =
        "INSERT INTO players (username) VALUES (?) "
        "ON DUPLICATE KEY UPDATE "
        "player_id = LAST_INSERT_ID(player_id)";

    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        fprintf(stderr, "Prepare failed: %s\n",
                mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return -1;
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = (void *)username;
    param[0].buffer_length = (unsigned long)username_length;

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

    printf("Player account loaded successfully. ID: %llu\n",
           player_id);

    return (int)player_id;
}

/* Close the database connection */
void db_close(void)
{
    if (connection != NULL) {
        mysql_close(connection);
        connection = NULL;
    }
}
