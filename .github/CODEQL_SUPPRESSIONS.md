# CodeQL Suppressions

This document records intentional CodeQL alerts that have been suppressed with explanations.

## Suppressions

### S307: Probable use of password hash as a password

**File:** `base_station/mqtt_client.py`
**Line:** 35
**Alert:** [CodeQL #6](https://github.com/gdellis/LoRaPaws32/security/code-scanning/6)

**Code:**
```python
self._client.username_pw_set(
    self._config.username,
    self._config.password,  # noqa: S307
)
```

**Reason:** The `password` parameter here is passed to paho-mqtt's `username_pw_set()`, which uses it for MQTT authentication. This is not a password hash being used as a password - it's a legitimate use of a password field for authentication. The CodeQL rule flags this as a false positive because it detects the variable name "password" being used in a function call, without considering the context.

The paho-mqtt library handles this credential securely by design and does not log it.
