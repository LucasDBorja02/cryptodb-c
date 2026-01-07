# Quick Start Guide

This guide will help you get cryptodb-c up and running in minutes.

## Prerequisites

- Linux/Unix system (Ubuntu/Debian recommended)
- GCC compiler
- PostgreSQL database server

## Installation Steps

### 1. Install Dependencies

```bash
sudo apt-get update
sudo apt-get install -y postgresql postgresql-contrib libpq-dev gcc make
```

### 2. Start PostgreSQL

```bash
sudo systemctl start postgresql
sudo systemctl enable postgresql
```

### 3. Clone and Build

```bash
# Clone the repository
git clone https://github.com/LucasDBorja02/cryptodb-c.git
cd cryptodb-c

# Build the project
make
```

### 4. Setup Database

```bash
# Create database and schema
sudo -u postgres psql -c "CREATE DATABASE cryptodb;"
sudo -u postgres psql cryptodb -f sql/schema.sql

# Optional: Set connection string
export DB_CONNINFO="host=localhost dbname=cryptodb user=postgres password=postgres"
```

## Basic Usage

### Encrypt Data

```bash
./cryptodb encrypt mydata "Secret information"
```

**Important**: Save the encryption key that is displayed!

Example output:
```
Data encrypted and stored successfully!
Name: mydata
Key (save this for decryption): a1b2c3d4e5f6...
```

### List Stored Records

```bash
./cryptodb list
```

Output:
```
Stored records:
Name                          Created At
----                          ----------
mydata                        2024-01-07 22:00:00
```

### Decrypt Data

```bash
./cryptodb decrypt mydata
```

Enter the key when prompted:
```
Enter decryption key (64 hex characters): a1b2c3d4e5f6...
Decrypted data: Secret information
```

## Troubleshooting

### "Connection to database failed"

1. Check PostgreSQL is running: `sudo systemctl status postgresql`
2. Verify database exists: `sudo -u postgres psql -l | grep cryptodb`
3. Check connection string in DB_CONNINFO

### "Failed to connect to database"

Ensure you have the correct credentials. Try:
```bash
sudo -u postgres psql cryptodb
```

If this works, set your connection string:
```bash
export DB_CONNINFO="host=localhost dbname=cryptodb user=postgres password=YOUR_PASSWORD"
```

### Compilation Errors

Ensure libpq-dev is installed:
```bash
sudo apt-get install libpq-dev
```

## What's Next?

- Read the [README.md](README.md) for complete documentation
- Check [TECHNICAL.md](TECHNICAL.md) for algorithm details
- See [CONTRIBUTING.md](CONTRIBUTING.md) to contribute

## Security Note

⚠️ This is an educational project. The encryption keys are displayed in plaintext for demonstration purposes. In production environments, use proper key management systems.

## Example Workflow

```bash
# 1. Encrypt user credentials
./cryptodb encrypt admin_password "SuperSecret123!"
# Save key: 4a5b6c7d8e9f0a1b2c3d4e5f6a7b8c9d0e1f2a3b4c5d6e7f8a9b0c1d2e3f4a5b

# 2. Encrypt API key
./cryptodb encrypt api_key "sk_live_1234567890abcdef"
# Save key: 9f8e7d6c5b4a39281716050403020100ffeeddccbbaa99887766554433221100

# 3. List all encrypted items
./cryptodb list

# 4. Decrypt when needed
./cryptodb decrypt admin_password
# Enter the saved key
```

## Support

For issues or questions, please open an issue on GitHub:
https://github.com/LucasDBorja02/cryptodb-c/issues
