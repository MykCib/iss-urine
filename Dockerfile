# Use the official Python image as a base
FROM python:3.10-slim

# Install system dependencies
RUN apt-get update && apt-get install -y \
    wget \
    unzip \
    curl \
    chromium-driver \
    chromium

# Set environment variables for Selenium
ENV CHROME_BIN=/usr/bin/chromium \
    CHROMEDRIVER_PATH=/usr/bin/chromedriver

# Set the working directory
WORKDIR /app

# Copy requirements and install Python dependencies
COPY requirements.txt requirements.txt
RUN pip install --no-cache-dir -r requirements.txt

# Copy the app code
COPY . .

# Expose the default port used by Cloud Run (8080)
EXPOSE 8080

# Start the FastAPI app with Uvicorn, using the PORT environment variable
CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8080"]
