---------------------------------------------------------------
-- Создание таблиц
---------------------------------------------------------------

CREATE TABLE Cinema (
    cinema_id SERIAL,
    name    VARCHAR(255)    NOT NULL,
    address VARCHAR(512)    NOT NULL,
    phone   VARCHAR(20)
);

CREATE TABLE Hall (
    hall_id                 SERIAL,
    cinema_id               INT     NOT NULL,
    regular_capacity        INT     NOT NULL,
    comfortable_capacity    INT     NOT NULL
);

CREATE TABLE Movie (
    movie_id    SERIAL,
    title       VARCHAR(255)    NOT NULL,
    genre       VARCHAR(100),
    director    VARCHAR(255),
    actors      TEXT[]
);

CREATE TABLE Screening (
    screening_id            SERIAL,
    movie_id                INT         NOT NULL,
    hall_id                 INT         NOT NULL,
    start_time              TIMESTAMP   NOT NULL,
    end_time                TIMESTAMP   NOT NULL,
    date                    DATE        NOT NULL,
    regular_available       INT         NOT NULL,
    comfortable_available   INT         NOT NULL
);

CREATE TABLE "User" (
    user_id     SERIAL,
    first_name  VARCHAR(100)    NOT NULL,
    last_name   VARCHAR(100)    NOT NULL,
    contact     VARCHAR(255)    NOT NULL
);

CREATE TABLE "Order" (
    order_id        SERIAL,
    user_id         INT             NOT NULL,
    screening_id    INT             NOT NULL,
    price           DECIMAL(10, 2)  NOT NULL
);

CREATE TABLE Ticket (
    ticket_id   SERIAL,
    order_id    INT             NOT NULL,
    seat_type   VARCHAR(20)     NOT NULL,
    quantity    INT             NOT NULL
);

---------------------------------------------------------------
-- Создание PK
---------------------------------------------------------------

ALTER TABLE Cinema ADD PRIMARY KEY (cinema_id);
ALTER TABLE Hall ADD PRIMARY KEY (hall_id);
ALTER TABLE Movie ADD PRIMARY KEY (movie_id);
ALTER TABLE Screening ADD PRIMARY KEY (screening_id);
ALTER TABLE "User" ADD PRIMARY KEY (user_id);
ALTER TABLE "Order" ADD PRIMARY KEY (order_id);
ALTER TABLE Ticket ADD PRIMARY KEY (ticket_id);

ALTER TABLE Hall
ADD CONSTRAINT fk_hall_cinema
FOREIGN KEY (cinema_id) REFERENCES Cinema(cinema_id);

ALTER TABLE Screening
ADD CONSTRAINT fk_screening_movie
FOREIGN KEY (movie_id) REFERENCES Movie(movie_id);

ALTER TABLE Screening
ADD CONSTRAINT fk_screening_hall
FOREIGN KEY (hall_id) REFERENCES Hall(hall_id);

ALTER TABLE "Order"
ADD CONSTRAINT fk_order_user
FOREIGN KEY (user_id) REFERENCES "User"(user_id);

ALTER TABLE "Order"
ADD CONSTRAINT fk_order_screening
FOREIGN KEY (screening_id) REFERENCES Screening(screening_id);

ALTER TABLE Ticket
ADD CONSTRAINT fk_ticket_order
FOREIGN KEY (order_id) REFERENCES "Order"(order_id);

---------------------------------------------------------------
-- Триггер validate_hall_capacity проверяет, чтобы значения обычной и комфортной вместимости
-- зала (regular_capacity, comfortable_capacity) не были отрицательными перед вставкой или
-- обновлением записей в таблице Hall.
---------------------------------------------------------------

CREATE OR REPLACE FUNCTION validate_hall_capacity()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.regular_capacity < 0 OR NEW.comfortable_capacity < 0 THEN
        RAISE EXCEPTION 'Capacity cannot be negative';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER hall_capacity_check
BEFORE INSERT OR UPDATE ON Hall
FOR EACH ROW EXECUTE FUNCTION validate_hall_capacity();

---------------------------------------------------------------
-- Триггер validate_screening_availability контролирует, чтобы количество доступных мест
-- на сеансе (regular_available, comfortable_available) оставалось неотрицательным.
---------------------------------------------------------------

CREATE OR REPLACE FUNCTION validate_screening_availability()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.regular_available < 0 OR NEW.comfortable_available < 0 THEN
        RAISE EXCEPTION 'Available seats cannot be negative';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER screening_availability_check
BEFORE INSERT OR UPDATE ON Screening
FOR EACH ROW EXECUTE FUNCTION validate_screening_availability();

---------------------------------------------------------------
-- Для таблицы Order триггер validate_order_price гарантирует,
-- что цена заказа (price) не может быть меньше нуля.
---------------------------------------------------------------

CREATE OR REPLACE FUNCTION validate_order_price()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.price < 0 THEN
        RAISE EXCEPTION 'Price cannot be negative';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER order_price_check
BEFORE INSERT OR UPDATE ON "Order"
FOR EACH ROW EXECUTE FUNCTION validate_order_price();

---------------------------------------------------------------
-- Триггер validate_ticket_data для таблицы Ticket проверяет тип места (seat_type)
-- на соответствие значениям regular или comfortable, а также контролирует,
-- чтобы количество билетов (quantity) было строго положительным.
---------------------------------------------------------------

CREATE OR REPLACE FUNCTION validate_ticket_data()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.seat_type NOT IN ('regular', 'comfortable') THEN
        RAISE EXCEPTION 'Invalid seat type';
    END IF;
    IF NEW.quantity <= 0 THEN
        RAISE EXCEPTION 'Quantity must be positive';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER ticket_data_check
BEFORE INSERT OR UPDATE ON Ticket
FOR EACH ROW EXECUTE FUNCTION validate_ticket_data();

---------------------------------------------------------------
-- Процедуры
---------------------------------------------------------------

-- Создает заказ, резервирует места на сеансе и создает билеты.
CREATE OR REPLACE PROCEDURE buy_tickets(
    p_user_id INT,
    p_screening_id INT,
    p_seat_type VARCHAR(20),
    p_quantity INT
)
LANGUAGE plpgsql
AS $$
DECLARE
    v_order_id INT;
    v_price DECIMAL(10, 2);
BEGIN
    -- Проверка доступности мест
    IF p_seat_type = 'regular' THEN
        UPDATE Screening
        SET regular_available = regular_available - p_quantity
        WHERE screening_id = p_screening_id
        AND regular_available >= p_quantity;
    ELSIF p_seat_type = 'comfortable' THEN
        UPDATE Screening
        SET comfortable_available = comfortable_available - p_quantity
        WHERE screening_id = p_screening_id
        AND comfortable_available >= p_quantity;
    ELSE
        RAISE EXCEPTION 'Invalid seat type';
    END IF;

    -- Если мест недостаточно, триггер вызовет ошибку
    -- Создание заказа
    INSERT INTO "Order" (user_id, screening_id, price)
    VALUES (p_user_id, p_screening_id, 0)
    RETURNING order_id INTO v_order_id;

    -- Расчет цены (условный пример: 500 руб. за обычное, 1000 за комфорт)
    v_price := CASE
        WHEN p_seat_type = 'regular' THEN 500 * p_quantity
        ELSE 1000 * p_quantity
    END;

    -- Обновление цены заказа
    UPDATE "Order" SET price = v_price WHERE order_id = v_order_id;

    -- Добавление билетов
    INSERT INTO Ticket (order_id, seat_type, quantity)
    VALUES (v_order_id, p_seat_type, p_quantity);

    COMMIT;
EXCEPTION
    WHEN others THEN
        ROLLBACK;
        RAISE;
END;
$$;

-- Пример:
-- CALL buy_tickets(1, 1, 'regular', 2);

-- Автоматически устанавливает доступные места
-- на основе вместимости зала.
CREATE OR REPLACE PROCEDURE add_screening(
    p_movie_id INT,
    p_hall_id INT,
    p_start_time TIMESTAMP,
    p_end_time TIMESTAMP,
    p_date DATE
)
LANGUAGE plpgsql
AS $$
DECLARE
    v_regular_capacity INT;
    v_comfortable_capacity INT;
BEGIN
    -- Получение вместимости зала
    SELECT regular_capacity, comfortable_capacity
    INTO v_regular_capacity, v_comfortable_capacity
    FROM Hall
    WHERE hall_id = p_hall_id;

    -- Добавление сеанса
    INSERT INTO Screening (
        movie_id,
        hall_id,
        start_time,
        end_time,
        date,
        regular_available,
        comfortable_available
    )
    VALUES (
        p_movie_id,
        p_hall_id,
        p_start_time,
        p_end_time,
        p_date,
        v_regular_capacity,
        v_comfortable_capacity
    );

    COMMIT;
END;
$$;

-- Пример:
-- CALL add_screening(2, 3, '2023-10-22 19:00:00', '2023-10-22 21:30:00', '2023-10-22');

-- Удаляет заказ и возвращает места в доступные.
CREATE OR REPLACE PROCEDURE cancel_order(p_order_id INT)
LANGUAGE plpgsql
AS $$
DECLARE
    v_screening_id INT;
    v_seat_type VARCHAR(20);
    v_quantity INT;
BEGIN
    -- Получение данных о билетах
    SELECT screening_id, seat_type, quantity
    INTO v_screening_id, v_seat_type, v_quantity
    FROM "Order" o
    JOIN Ticket t ON o.order_id = t.order_id
    WHERE o.order_id = p_order_id;

    -- Возврат мест
    IF v_seat_type = 'regular' THEN
        UPDATE Screening
        SET regular_available = regular_available + v_quantity
        WHERE screening_id = v_screening_id;
    ELSE
        UPDATE Screening
        SET comfortable_available = comfortable_available + v_quantity
        WHERE screening_id = v_screening_id;
    END IF;

    -- Удаление заказа и билетов (CASCADE)
    DELETE FROM "Order" WHERE order_id = p_order_id;

    COMMIT;
EXCEPTION
    WHEN others THEN
        ROLLBACK;
        RAISE;
END;
$$;

-- Пример:
-- CALL cancel_order(1);

---------------------------------------------------------------
-- Представления
---------------------------------------------------------------

-- Показывает расписание сеансов с названием фильма, кинотеатра, зала и временем показа.
CREATE VIEW vw_screening_details AS
SELECT
    s.screening_id,
    m.title AS movie_title,
    c.name AS cinema_name,
    h.hall_id,
    s.start_time,
    s.end_time,
    s.date,
    s.regular_available,
    s.comfortable_available
FROM Screening s
JOIN Movie m ON s.movie_id = m.movie_id
JOIN Hall h ON s.hall_id = h.hall_id
JOIN Cinema c ON h.cinema_id = c.cinema_id;

-- Отображает все заказы пользователей с деталями билетов.
CREATE VIEW vw_user_orders AS
SELECT
    u.user_id,
    u.first_name || ' ' || u.last_name AS user_full_name,
    o.order_id,
    o.price AS total_price,
    t.seat_type,
    t.quantity,
    m.title AS movie_title,
    s.start_time
FROM "Order" o
JOIN "User" u ON o.user_id = u.user_id
JOIN Ticket t ON o.order_id = t.order_id
JOIN Screening s ON o.screening_id = s.screening_id
JOIN Movie m ON s.movie_id = m.movie_id;

-- Показывает количество оставшихся мест для каждого сеанса.
CREATE VIEW vw_available_seats AS
SELECT
    s.screening_id,
    m.title AS movie_title,
    h.hall_id,
    s.regular_available,
    s.comfortable_available,
    h.regular_capacity - s.regular_available AS regular_sold,
    h.comfortable_capacity - s.comfortable_available AS comfortable_sold
FROM Screening s
JOIN Hall h ON s.hall_id = h.hall_id
JOIN Movie m ON s.movie_id = m.movie_id;

---------------------------------------------------------------
-- Создание индексов
---------------------------------------------------------------

CREATE INDEX idx_screening_movie ON Screening(movie_id);
CREATE INDEX idx_screening_hall ON Screening(hall_id);
CREATE INDEX idx_order_user ON "Order"(user_id);
CREATE INDEX idx_ticket_order ON Ticket(order_id);

---------------------------------------------------------------
-- Заполнение
---------------------------------------------------------------

-- Кинотеатры
INSERT INTO Cinema (name, address, phone)
VALUES
    ('Киномакс', 'ул. Пушкина, 15', '+79991234567'),
    ('Синема Парк', 'пр. Ленина, 42', '+79997654321'),
    ('IMAX Планета', 'ул. Гагарина, 7', '+79035557788');

-- Залы
INSERT INTO Hall (cinema_id, regular_capacity, comfortable_capacity)
VALUES
    (1, 120, 30),
    (1, 80, 20),
    (1, 150, 40),
    (2, 200, 50),
    (2, 180, 60),
    (3, 300, 100),
    (3, 400, 200);

-- Фильмы
INSERT INTO Movie (title, genre, director, actors)
VALUES
    ('Форсаж 10', 'боевик', 'Луи Летерье', ARRAY['Вин Дизель']),
    ('Стражи Галактики 3', 'фантастика', 'Джеймс Ганн', ARRAY['Крис Пратт']),
    ('Аватар 2', 'фэнтези', 'Джеймс Кэмерон', ARRAY['Сэм Уортингтон']),
    ('Оппенгеймер', 'драма', 'Кристофер Нолан', ARRAY['Киллиан Мерфи']);

-- Сеансы (на месяц вперед)
INSERT INTO Screening (movie_id, hall_id, start_time, end_time, date, regular_available, comfortable_available)
VALUES
    -- Форсаж 10 (3 зала)
    (1, 1, '2024-03-01 18:00:00', '2024-03-01 20:30:00', '2024-03-01', 120, 30),
    (1, 2, '2024-03-02 15:00:00', '2024-03-02 17:30:00', '2024-03-02', 80, 20),
    (1, 3, '2024-03-03 20:00:00', '2024-03-03 22:30:00', '2024-03-03', 150, 40),
    -- Стражи Галактики (2 зала)
    (2, 4, '2024-03-01 12:00:00', '2024-03-01 14:30:00', '2024-03-01', 200, 50),
    (2, 5, '2024-03-02 19:00:00', '2024-03-02 21:30:00', '2024-03-02', 180, 60),
    -- Аватар 2 (1 зал)
    (3, 6, '2024-03-05 10:00:00', '2024-03-05 13:00:00', '2024-03-05', 300, 100);

-- Пользователи
INSERT INTO "User" (first_name, last_name, contact)
VALUES
    ('Иван', 'Иванов', 'ivan@mail.ru'),
    ('Мария', 'Петрова', 'maria@gmail.com'),
    ('Алексей', 'Сидоров', 'alex@yandex.ru');

-- Заказы и билеты
INSERT INTO "Order" (user_id, screening_id, price)
VALUES
    -- Иван купил билеты на все сеансы Форсажа (запрос 6)
    (1, 1, 1500),
    (1, 2, 2000),
    (1, 3, 3000),
    -- Мария купила билеты в разных кинотеатрах (запрос 11)
    (2, 4, 2500),  -- Синема Парк
    (2, 6, 4000),  -- IMAX
    -- Алексей купил билеты за последние 7 дней (запрос 4)
    (3, 1, 1000);

INSERT INTO Ticket (order_id, seat_type, quantity)
VALUES
    (1, 'regular', 2),
    (1, 'comfortable', 1),
    (2, 'regular', 3),
    (3, 'comfortable', 2),
    (4, 'regular', 4),
    (5, 'comfortable', 2),
    (6, 'regular', 1);

---------------------------------------------------------------
-- Запросы
---------------------------------------------------------------

--(01)Количество комфортных мест в первом зале первого кинотеатра
SELECT h.comfortable_capacity
FROM Hall h
JOIN Cinema c ON h.cinema_id = c.cinema_id
WHERE c.cinema_id = 1 AND h.hall_id = 1;

--(02)Фильмы, которые показываются в кинотеатрах с более чем 2 залами
SELECT m.title
FROM Movie m
JOIN Screening s ON m.movie_id = s.movie_id
JOIN Hall h ON s.hall_id = h.hall_id
JOIN Cinema c ON h.cinema_id = c.cinema_id
GROUP BY m.movie_id
HAVING COUNT(DISTINCT h.hall_id) > 2;

--(03)Средняя сумма заказов для каждого пользователя
SELECT
    u.user_id,
    u.first_name || ' ' || u.last_name AS user_name,
    ROUND(AVG(o.price), 2) AS avg_order_price
FROM "User" u
JOIN "Order" o ON u.user_id = o.user_id
GROUP BY u.user_id;

--(04)Количество проданных билетов по типам мест за последние 7 дней
SELECT
    t.seat_type,
    SUM(t.quantity) AS total_tickets
FROM Ticket t
JOIN "Order" o ON t.order_id = o.order_id
JOIN Screening s ON o.screening_id = s.screening_id
WHERE s.date BETWEEN '2024-03-01' AND '2024-03-07'
GROUP BY t.seat_type;

--(05)Кинотеатры с общей вместимостью залов > 500 мест
SELECT
    c.name,
    SUM(h.regular_capacity + h.comfortable_capacity) AS total_capacity
FROM Cinema c
JOIN Hall h ON c.cinema_id = h.cinema_id
GROUP BY c.cinema_id
HAVING SUM(h.regular_capacity + h.comfortable_capacity) > 500;

--(06)Пользователи, купившие билеты на все сеансы фильма "Форсаж 10"
SELECT u.user_id, u.first_name, u.last_name
FROM "User" u
WHERE NOT EXISTS (
    SELECT s.screening_id
    FROM Screening s
    JOIN Movie m ON s.movie_id = m.movie_id
    WHERE m.title = 'Форсаж 10'
    EXCEPT
    SELECT o.screening_id
    FROM "Order" o
    WHERE o.user_id = u.user_id
);

--(07)Топ-3 фильма по количеству сеансов
SELECT
    m.title,
    COUNT(s.screening_id) AS total_screenings
FROM Movie m
LEFT JOIN Screening s ON m.movie_id = s.movie_id
GROUP BY m.movie_id
ORDER BY total_screenings DESC
LIMIT 3;

--(08)Доход кинотеатров за текущий месяц
SELECT
    c.name,
    SUM(o.price) AS total_revenue
FROM Cinema c
JOIN Hall h ON c.cinema_id = h.cinema_id
JOIN Screening s ON h.hall_id = s.hall_id
JOIN "Order" o ON s.screening_id = o.screening_id
WHERE EXTRACT(MONTH FROM s.date) = 3  -- Март
GROUP BY c.cinema_id;

--(09)Залы, в которых не было сеансов
SELECT h.hall_id, c.name AS cinema_name
FROM Hall h
LEFT JOIN Screening s ON h.hall_id = s.hall_id
JOIN Cinema c ON h.cinema_id = c.cinema_id
WHERE s.screening_id IS NULL;

--(10)Расписание сеансов с указанием свободных мест
SELECT
    s.screening_id,
    m.title,
    s.regular_available || '/' || h.regular_capacity AS regular_seats,
    s.comfortable_available || '/' || h.comfortable_capacity AS comfortable_seats
FROM Screening s
JOIN Hall h ON s.hall_id = h.hall_id
JOIN Movie m ON s.movie_id = m.movie_id;

--(11)Пользователи, посетившие более одного кинотеатра
SELECT
    u.user_id,
    u.first_name || ' ' || u.last_name AS user_name
FROM "User" u
JOIN "Order" o ON u.user_id = o.user_id
JOIN Screening s ON o.screening_id = s.screening_id
JOIN Hall h ON s.hall_id = h.hall_id
GROUP BY u.user_id
HAVING COUNT(DISTINCT h.cinema_id) > 1;

--(12)Динамика продаж по дням за последний месяц
SELECT
    s.date,
    SUM(t.quantity) AS total_tickets
FROM Screening s
JOIN "Order" o ON s.screening_id = o.screening_id
JOIN Ticket t ON o.order_id = t.order_id
WHERE s.date BETWEEN '2024-03-01' AND '2024-03-31'
GROUP BY s.date
ORDER BY s.date;