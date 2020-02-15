# php-tulind
Tulind bindings

# Install
```bash
sudo make install

# Build
```bash
sudo make build
```

# Usage
```php
$tulind = new Tulind();

echo "version:" . $tulind->version();
echo "\r\n";

$tulind->sma([1,2,3,4,5,6,7,8,9,10], [10]));
$tulind->ema([25,24.875,24.781,24.594,24.5,24.625,25.219,27.25], [5]);
```
