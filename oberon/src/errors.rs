use std::{fmt, num::ParseIntError, str::Utf8Error};

use redis::RedisError;

pub enum Error {
    ClientConnectionTerminated,
    ClientTimeout,
    ClientRefused,
    ClientOther,
    InputValidationError,
    ParseError,
}

pub type OberonResult<T> = Result<T, Error>;

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Error display is not yet implemented!")
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
        Error::InputValidationError
    }
}

impl From<ParseIntError> for Error {
    fn from(_err: ParseIntError) -> Self {
        Error::ParseError
    }
}
