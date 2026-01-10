package server

import (
	"net/http"
	"simplews/handlers"
	"simplews/ws"

	"golang.org/x/net/websocket"
)

func registerRoutes(mux *http.ServeMux) {
	hub := ws.NewHub()

	mux.HandleFunc("/", handlers.Index)

	mux.Handle("/ws",
		websocket.Handler(hub.HandleWS),
	)

	mux.Handle("/static/",
		http.StripPrefix("/static/",
			http.FileServer(http.Dir("static")),
		),
	)
}
