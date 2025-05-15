CREATE TABLE Cinema (
    cinema_id SERIAL PRIMARY KEY,
    name      VARCHAR(255)  NOT NULL,
    address   VARCHAR(512)  NOT NULL,
    phone     VARCHAR(20)
);

CREATE TABLE Hall (
    hall_id              SERIAL PRIMARY KEY,
    cinema_id            INT NOT NULL REFERENCES Cinema(cinema_id),
    regular_capacity     INT NOT NULL CHECK (regular_capacity >= 0),
    comfortable_capacity INT NOT NULL CHECK (comfortable_capacity >= 0)
);

CREATE TABLE Movie (
    movie_id    SERIAL PRIMARY KEY,
    title       VARCHAR(255) NOT NULL,
    genre       VARCHAR(100),
    director    VARCHAR(255),
    actors      TEXT[]
);

CREATE TABLE Screening (
    screening_id          SERIAL PRIMARY KEY,
    movie_id              INT NOT NULL REFERENCES Movie(movie_id),
    hall_id               INT NOT NULL REFERENCES Hall(hall_id),
    start_time            TIMESTAMP NOT NULL,
    end_time              TIMESTAMP NOT NULL,
    date                  DATE NOT NULL,
    regular_available     INT NOT NULL CHECK (regular_available >= 0),
    comfortable_available INT NOT NULL CHECK (comfortable_available >= 0)
);

CREATE TABLE "User" (
    user_id     SERIAL PRIMARY KEY,
    first_name  VARCHAR(100) NOT NULL,
    last_name   VARCHAR(100) NOT NULL,
    contact     VARCHAR(255) NOT NULL
);

CREATE TABLE "Order" (
    order_id        SERIAL PRIMARY KEY,
    user_id         INT NOT NULL REFERENCES "User"(user_id),
    screening_id    INT NOT NULL REFERENCES Screening(screening_id),
    price           DECIMAL(10, 2) NOT NULL CHECK (price >= 0)
);

CREATE TABLE Ticket (
    ticket_id   SERIAL PRIMARY KEY,
    order_id    INT NOT NULL REFERENCES "Order"(order_id),
    seat_type   VARCHAR(20) NOT NULL CHECK (seat_type IN ('regular', 'comfortable')),
    quantity    INT NOT NULL CHECK (quantity > 0)
);

CREATE INDEX idx_screening_movie ON Screening(movie_id);
CREATE INDEX idx_screening_hall ON Screening(hall_id);
CREATE INDEX idx_order_user ON "Order"(user_id);
CREATE INDEX idx_ticket_order ON Ticket(order_id);

INSERT INTO Cinema (name, address, phone)
VALUES ('Киномакс', 'ул. Пушкина, 15', '+79991234567');

INSERT INTO Hall (cinema_id, regular_capacity, comfortable_capacity)
VALUES (1, 150, 50);

INSERT INTO Movie (title, genre, director, actors)
VALUES (
    'Форсаж 10',
    'боевик',
    'Луи Летерье',
    ARRAY['Вин Дизель', 'Джейсон Стейтем']
);