use std::{fmt, num::ParseIntError, str::Utf8Error};

use redis::RedisError;

pub enum Error {
    ClientConnectionTerminated,
    ClientTimeout,
    ClientRefused,
    ClientOther,
    InputValidationError(String),
    ParseError(Box<dyn std::error::Error>),
    PermissionDenied,
    FileNotFound,
    UncategorizedError,
}

pub type OberonResult<T> = Result<T, Error>;

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Error::ClientConnectionTerminated => {
                write!(f, "Repository connection dropped for Oberon client")
            }
            Error::ClientTimeout => {
                write!(f, "Oberon client timeout when connecting to repository")
            }
            Error::ClientRefused => {
                write!(f, "Repository refused connection for Oberon client")
            }
            Error::InputValidationError(s) => {
                write!(f, "Input validation error {}", s)
            }
            Error::ParseError(err) => {
                write!(f, "Parse error: {}", err)
            }
            Error::PermissionDenied => {
                write!(f, "Permission denied")
            }
            _ => {
                write!(f, "{} error", self)
            }
        }
    }
}

impl fmt::Debug for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self)
    }
}

impl From<RedisError> for Error {
    #[inline]
    fn from(err: RedisError) -> Error {
        if err.is_timeout() {
            return Error::ClientTimeout;
        }
        if err.is_connection_refusal() {
            return Error::ClientRefused;
        }
        if err.is_connection_dropped() {
            return Error::ClientConnectionTerminated;
        }
        Error::ClientOther
    }
}

impl From<Utf8Error> for Error {
    #[inline]
    fn from(_err: Utf8Error) -> Error {
        Error::InputValidationError("Utf8Error".to_string())
    }
}

impl From<ParseIntError> for Error {
    fn from(err: ParseIntError) -> Self {
        Error::ParseError(Box::new(err))
    }
}

impl From<std::io::Error> for Error {
    fn from(err: std::io::Error) -> Self {
        match err.kind() {
            std::io::ErrorKind::NotFound => Error::FileNotFound,
            std::io::ErrorKind::PermissionDenied => Error::PermissionDenied,
            _ => Error::UncategorizedError,
        }
    }
}
