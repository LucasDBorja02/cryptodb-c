# Contributing to cryptodb-c

Thank you for your interest in contributing to cryptodb-c!

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue with:
- A clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- System information (OS, PostgreSQL version, etc.)

### Suggesting Enhancements

Enhancement suggestions are welcome! Please provide:
- A clear description of the enhancement
- Use cases and benefits
- Any implementation ideas

### Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Test your changes thoroughly
5. Commit with clear messages (`git commit -m 'Add amazing feature'`)
6. Push to your branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

### Code Guidelines

- Follow the existing code style
- Add comments for complex logic
- Update documentation as needed
- Ensure code compiles without warnings
- Test with various inputs

### Security

If you discover a security vulnerability, please email the maintainer directly rather than opening a public issue.

## Development Setup

1. Clone the repository
2. Install dependencies: `make install-deps`
3. Setup database: `make setup-db`
4. Build: `make`
5. Test your changes

## Testing

Currently, testing is manual. To test:

1. Encrypt data with various inputs
2. Decrypt and verify correctness
3. Test edge cases (empty strings, long texts, special characters)
4. Verify database operations

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
