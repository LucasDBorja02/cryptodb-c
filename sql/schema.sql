-- Database schema for cryptodb-c
-- Stores encrypted data with IVs

-- Create database (run manually if needed)
-- CREATE DATABASE cryptodb;

-- Connect to the database
-- \c cryptodb;

-- Create table for encrypted data
CREATE TABLE IF NOT EXISTS encrypted_data (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) UNIQUE NOT NULL,
    ciphertext TEXT NOT NULL,
    iv VARCHAR(32) NOT NULL,  -- 16 bytes as hex = 32 characters
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create index on name for faster lookups
CREATE INDEX IF NOT EXISTS idx_encrypted_data_name ON encrypted_data(name);

-- Display table structure
\d encrypted_data
