---------------------------------------------------------------
-- Удаление
---------------------------------------------------------------

DROP VIEW IF EXISTS
    vw_screening_details,
    vw_user_orders,
    vw_available_seats;

DROP TABLE IF EXISTS Ticket CASCADE;
DROP TABLE IF EXISTS "Order" CASCADE;
DROP TABLE IF EXISTS "User" CASCADE;
DROP TABLE IF EXISTS Screening CASCADE;
DROP TABLE IF EXISTS Movie CASCADE;
DROP TABLE IF EXISTS Hall CASCADE;
DROP TABLE IF EXISTS Cinema CASCADE;