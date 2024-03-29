
CREATE TABLE IF NOT EXISTS app_user (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY ,
  nickname VARCHAR(256) NOT NULL ,
  mail VARCHAR(256) NOT NULL , 
  password VARCHAR(256) NOT NULL,
  CONSTRAINT UK_APPUSER_EMAIL UNIQUE (mail)
  
   );

  
CREATE TABLE IF NOT EXISTS chat (
  id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY ,
  user_first INT NOT NULL ,
  user_second INT NOT NULL ,
  CONSTRAINT fk_user_first
    FOREIGN KEY (user_first )
    REFERENCES app_user (id)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION ,
  CONSTRAINT fk_user_second
    FOREIGN KEY (user_second )
    REFERENCES app_user (id)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION

  );


CREATE TABLE IF NOT EXISTS user_token (
  id INT PRIMARY KEY REFERENCES app_user (id),
  token VARCHAR(256) NOT NULL,
  expiry INT NOT NULL ,
  CONSTRAINT UK_TOKEN UNIQUE (token)

);

CREATE TABLE IF NOT EXISTS chat_line (
  id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
  chat_id INT NOT NULL ,
  user_id INT NOT NULL ,
  line_text TEXT NOT NULL ,
  created_at INT NOT NULL ,
  is_read BOOLEAN NOT NULL DEFAULT FALSE ,
  message_hash INT NOT NULL ,
  CONSTRAINT fk_chat_line_chat
    FOREIGN KEY (chat_id )
    REFERENCES chat (id )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION ,
  CONSTRAINT fk_chat_line_chat_user1
    FOREIGN KEY (user_id )
    REFERENCES app_user (id )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION
    );

