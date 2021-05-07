# Clean the old data
rm -rf ../arduino/garage_bot/data/*

# Create some basic folders required for the copy
mkdir ../arduino/garage_bot/data

# Copy the files to the data directory
cp -r dist/* ../arduino/garage_bot/data 2>/dev/null