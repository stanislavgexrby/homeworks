---------------------------------------------------------------
-- Удаление
---------------------------------------------------------------

-- Удаление представлений
DROP VIEW IF EXISTS
    vw_screening_details,
    vw_user_orders,
    vw_available_seats CASCADE;

-- Удаление триггеров (по одному)
DROP TRIGGER IF EXISTS hall_capacity_check ON Hall CASCADE;
DROP TRIGGER IF EXISTS screening_availability_check ON Screening CASCADE;
DROP TRIGGER IF EXISTS order_price_check ON "Order" CASCADE;
DROP TRIGGER IF EXISTS ticket_data_check ON Ticket CASCADE;

-- Удаление функций
DROP FUNCTION IF EXISTS
    validate_hall_capacity(),
    validate_screening_availability(),
    validate_order_price(),
    validate_ticket_data() CASCADE;

-- Удаление таблиц
DROP TABLE IF EXISTS
    Ticket,
    "Order",
    "User",
    Screening,
    Movie,
    Hall,
    Cinema CASCADE;