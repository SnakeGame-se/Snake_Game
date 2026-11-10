
# Snake Game - MySQL Database Setup

## Requirements
- MySQL Server
- MySQL C Connector
- GCC compiler

## Database creation

Run the SQL file:

    SOURCE schema.sql;

This creates the snake_game database with:
- players
- game_history

## Environment variables

Configure these variables before running the game:

- SNAKE_DB_HOST: localhost
- SNAKE_DB_PORT: 3306
- SNAKE_DB_NAME: snake_game
- SNAKE_DB_USER: Your MySQL username
- SNAKE_DB_PASSWORD: Your MySQL password

Never commit database passwords to GitHub.

## Database features

- Username-based player registration
- Existing player lookup
- Individual game score history
- Automatic high-score updates
- Games played tracking
- Global top-10 leaderboard
