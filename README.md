# php-tulind
Tulind bindings

# Install
```bash
git clone git@github.com:anthHugo/php-tulind.git
cd php-tulind/
sudo make install
```

# Build
```bash
sudo make build
```

# Usage
```php
$tulind = new Tulind();

// Display tulind version
$tulind->version();

// Get all indicators
$tulind->indicators();

// Apply indicators on data
$tulind->sma([1,2,3,4,5,6,7,8,9,10], [10]));
$tulind->ema([25,24.875,24.781,24.594,24.5,24.625,25.219,27.25], [5]);
```
