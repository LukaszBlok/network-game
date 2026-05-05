# Project 2 - Network Game "Race to 50"

A two-player network game played over UDP sockets. Players take turns adding numbers, and the one who reaches exactly 50 wins the round.

## How the Game Works

1. The first player to launch the program becomes the **client** and connects to the second player
2. The server draws a random starting number (1-10)
3. Players alternate turns - each player must submit a number that is:
   - greater than the current number
   - at most 10 higher than the current number
   - between 1 and 50
4. The player who reaches **exactly 50** wins the round
5. After each round, roles swap - the winner becomes the client, the loser becomes the server
6. The score is tracked across rounds

## Compilation

```bash
gcc game.c -o game
```

## Usage

Both players must run the same program. The **first player** to launch it becomes the client and connects to the second player's machine.

```bash
./game <ip_or_hostname> <port> [nick]
```

- `<ip_or_hostname>` - IP address or hostname of the opponent's machine
- `<port>` - port number (must be greater than 1000)
- `[nick]` - optional nickname (if not provided, the local IP address is used)

### Example

Player 1 (connects to Player 2 at 192.168.1.10):
```bash
./game 192.168.1.10 5000 Alice
```

Player 2 (connects to Player 1 at 192.168.1.5):
```bash
./game 192.168.1.5 5000 Bob
```

> Both players must use the same port number.

## In-Game Commands

During your turn you can type:

| Command | Description |
|---------|-------------|
| `<number>` | Submit a number (must be current value +1 to +10, max 50) |
| `wynik` | Display current score (wins : losses) |
| `koniec` | End the game and notify the opponent |

## Game Flow

- Both players connect to each other using the same port
- The program automatically handles role switching (server/client) after each round
- When a round ends, the loser waits and becomes the new server for the next round
- When the opponent ends the game, you are asked whether you want to start a new game (`tak`) or quit (`koniec`)

## Important Notes

- Both players must be reachable on the network (same LAN or with port forwarding)
- The port must be greater than 1000
- If no nickname is provided, the local machine's IP address is used as the nick
- The program uses **UDP** (connectionless) sockets
