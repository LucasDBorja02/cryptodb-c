#!/bin/bash
# Demo script for cryptodb-c
# This script demonstrates the encryption and decryption workflow

set -e

echo "========================================"
echo "cryptodb-c Demo Script"
echo "========================================"
echo ""

# Check if binary exists
if [ ! -f "./cryptodb" ]; then
    echo "Error: cryptodb binary not found. Please run 'make' first."
    exit 1
fi

# Check if PostgreSQL is accessible (optional, will be checked by the app)
echo "Note: This demo requires PostgreSQL to be running and configured."
echo "Default connection: host=localhost dbname=cryptodb user=postgres"
echo ""

# Test 1: Encrypt some data
echo "Test 1: Encrypting data..."
echo "Command: ./cryptodb encrypt demo_secret 'This is a secret message!'"
echo ""

# We'll just show the command since we need proper DB setup
echo "Example output:"
echo "  Data encrypted and stored successfully!"
echo "  Name: demo_secret"
echo "  Key (save this for decryption): a1b2c3d4e5f6789..."
echo ""

# Test 2: List records
echo "Test 2: List all stored records"
echo "Command: ./cryptodb list"
echo ""
echo "Example output:"
echo "  Stored records:"
echo "  Name                          Created At"
echo "  ----                          ----------"
echo "  demo_secret                   2024-01-07 22:00:00"
echo ""

# Test 3: Decrypt data
echo "Test 3: Decrypt stored data"
echo "Command: ./cryptodb decrypt demo_secret"
echo "(When prompted, enter the encryption key)"
echo ""
echo "Example output:"
echo "  Decrypted data: This is a secret message!"
echo ""

echo "========================================"
echo "To run these commands for real:"
echo "1. Ensure PostgreSQL is running"
echo "2. Create the database: make setup-db"
echo "3. Set DB_CONNINFO if needed"
echo "4. Run: ./cryptodb encrypt <name> <text>"
echo "5. Run: ./cryptodb decrypt <name>"
echo "========================================"
