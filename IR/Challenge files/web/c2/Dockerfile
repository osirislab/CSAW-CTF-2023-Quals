FROM python:3.9
WORKDIR /web
COPY requirements.txt /web/requirements.txt
RUN pip install --no-cache-dir --upgrade -r /web/requirements.txt
COPY . /web
CMD ["uvicorn", "web:app", "--host", "0.0.0.0", "--port", "8080", "--reload"]