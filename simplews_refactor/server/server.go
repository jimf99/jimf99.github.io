package server

import (
	"log"
	"net/http"
)

func Start(addr string) {
	mux := http.NewServeMux()
	registerRoutes(mux)

	log.Println("Server listening on", addr)
	log.Fatal(http.ListenAndServe(addr, mux))
}
