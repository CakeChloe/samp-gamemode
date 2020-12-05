-- phpMyAdmin SQL Dump
-- version 4.9.0.1
-- https://www.phpmyadmin.net/
--
-- Värd: 127.0.0.1
-- Tid vid skapande: 05 dec 2020 kl 18:31
-- Serverversion: 10.4.6-MariaDB
-- PHP-version: 7.1.32

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Databas: `lsrp`
--

-- --------------------------------------------------------

--
-- Tabellstruktur `accounts`
--

CREATE TABLE `accounts` (
  `id` int(11) NOT NULL,
  `name` varchar(24) NOT NULL,
  `password` varchar(61) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumpning av Data i tabell `accounts`
--

INSERT INTO `accounts` (`id`, `name`, `password`) VALUES
(2, 'Cake', '$2y$12$QyGwYU7oXTPOOjPHaynmQeCOntUJG3GHYgp.xR3Wijo6h.mX8iofS'),
(3, 'Cakey', '$2y$12$KFPvQlXJRCvMbBDwUSXpQuuWgtibvCbpC9mWqeX7WNi6aPmF2zzS2');

-- --------------------------------------------------------

--
-- Tabellstruktur `characters`
--

CREATE TABLE `characters` (
  `id` int(11) NOT NULL,
  `account` int(11) NOT NULL,
  `firstname` varchar(24) NOT NULL,
  `lastname` varchar(24) NOT NULL,
  `skin` int(11) NOT NULL DEFAULT 1,
  `breadth` float NOT NULL DEFAULT 1466.21,
  `height` float NOT NULL DEFAULT -898.66,
  `depth` float NOT NULL DEFAULT 54.8359,
  `angle` float NOT NULL DEFAULT 2.8316,
  `health` float NOT NULL DEFAULT 100,
  `armour` float NOT NULL DEFAULT 0,
  `interior` int(11) NOT NULL DEFAULT 0,
  `world` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumpning av Data i tabell `characters`
--

INSERT INTO `characters` (`id`, `account`, `firstname`, `lastname`, `skin`, `breadth`, `height`, `depth`, `angle`, `health`, `armour`, `interior`, `world`) VALUES
(1, 2, 'dffsd', 'dsf', 1, 1466.21, -898.66, 54.8359, 2.8316, 100, 0, 0, 0),
(2, 2, 'Hannah', 'Rosenwaike', 1, 1466.21, -898.66, 54.8359, 2.8316, 100, 0, 0, 0),
(3, 3, 'Cake', 'Test', 1, 1446.95, -921.261, 48.3529, 156.617, 100, 0, 0, 0),
(4, 3, 'Emma', 'Portland', 1, 1465.97, -898.292, 54.8359, 2.83158, 100, 0, 0, 0);

--
-- Index för dumpade tabeller
--

--
-- Index för tabell `accounts`
--
ALTER TABLE `accounts`
  ADD PRIMARY KEY (`id`);

--
-- Index för tabell `characters`
--
ALTER TABLE `characters`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT för dumpade tabeller
--

--
-- AUTO_INCREMENT för tabell `accounts`
--
ALTER TABLE `accounts`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT för tabell `characters`
--
ALTER TABLE `characters`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
