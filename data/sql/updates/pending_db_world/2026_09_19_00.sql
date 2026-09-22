-- Project Ambrose by Imjustchico
-- C-01 authored Wizard City door destinations named by the roadmap, with integrity constraints.
CREATE TABLE IF NOT EXISTS `zone_teleport` (
    `zone` VARCHAR(128) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    `trigger_name` VARCHAR(128) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    `dest_zone` VARCHAR(128) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
    `dest_location` VARCHAR(128) CHARACTER SET ascii COLLATE ascii_bin NOT NULL DEFAULT '',
    `transition_id` INT UNSIGNED NOT NULL DEFAULT 0,
    `same_zone` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`zone`, `trigger_name`),
    CONSTRAINT `chk_zone_teleport_same_zone`
        CHECK (`same_zone` IN (0, 1)),
    CONSTRAINT `chk_zone_teleport_same_destination`
        CHECK (`same_zone` = 0 OR `dest_zone` = `zone`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT INTO `zone_teleport`
    (`zone`, `trigger_name`, `dest_zone`, `dest_location`, `transition_id`, `same_zone`)
VALUES
    ('WC_Hub', 'TeleportToRavenwoodTrigger', 'Ravenwood', '', 0, 0),
    ('WC_Hub', 'TeleportToShoppingDistrict', 'Shopping District', '', 0, 0);
