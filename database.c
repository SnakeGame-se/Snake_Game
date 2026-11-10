
#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

static MYSQL *connection = NULL;

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

void db_close(void)
{
    if (connection != NULL) {
        mysql_close(connection);
        connection = NULL;
    }
}
