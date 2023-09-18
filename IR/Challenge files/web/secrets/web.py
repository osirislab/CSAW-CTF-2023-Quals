import uvicorn
from fastapi import FastAPI

from settings import KEY

app = FastAPI()


@app.get("/key")
async def key():
    return KEY

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8080)
