-- database: /workspaces/ChessSocketServer/database.db

-- Tạo bảng User
CREATE TABLE IF NOT EXISTS user (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username VARCHAR,
  password VARCHAR,
  elo INTEGER
);

-- Tạo bảng Friend
CREATE TABLE IF NOT EXISTS friend (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  user_id INTEGER,
  friend_id INTEGER,
  FOREIGN KEY (user_id) REFERENCES user(id),
  FOREIGN KEY (friend_id) REFERENCES user(id)
);

-- Tạo bảng Game
CREATE TABLE "room" (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  black_user_id INTEGER,
  white_user_id INTEGER,
  total_time INTEGER,
  winer_user INTEGER, 
  start_time DATE, 
  end_time DATE,
  FOREIGN KEY (black_user_id) REFERENCES user(id),
  FOREIGN KEY (white_user_id) REFERENCES user(id)
)
-- Tạo bảng piece
CREATE TABLE piece (
  pieceId   INTEGER PRIMARY KEY,
  type      CHAR(1),
  colour    CHAR(1)
)
-- Tạo bảng move
CREATE TABLE move (
  move_id    INTEGER PRIMARY KEY AUTOINCREMENT,
  room_id    INTEGER,
  piece_id   INTEGER,
    from_x     FLOAT,
  from_y  FLOAT,
  to_x     FLOAT,
  to_y  FLOAT,
  UNIQUE(room_id, piece_id, move_id), -- Đảm bảo mỗi cặp (room_id, piece_id) chỉ xuất hiện một lần cho mỗi MoveId
  FOREIGN KEY (room_id) REFERENCES room(id)
)
-- Lịch sử đấu:
SELECT 
	room.id,
	total_time,
	user.username AS opponent_name,
	start_time,
	end_time,
CASE
	WHEN winer_user = 2 THEN 1 
    WHEN winer_user = 0 THEN 2
	ELSE 0 
	END AS game_result 
FROM
	room JOIN user ON((room.black_user_id = user.id OR room.white_user_id = user.id ) AND user.id != 2)
WHERE
	( black_user_id = 2 OR white_user_id = 2 );