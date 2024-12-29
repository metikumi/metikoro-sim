
# Software Notes

## State Simplification

In order to simplify game state comparison and linking of nodes:

- Player 0 is always the current player in each game state.
- The board is rotated 90º clockwise after each move, to make sure Player 0 is the current player.
- That means that contradicting to the original rules, the player turns are counter-clockwise (which has no influence on the game.)

