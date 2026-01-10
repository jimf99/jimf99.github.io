package ws

import (
	"log"

	"golang.org/x/net/websocket"
)

type Hub struct {
	clients map[*websocket.Conn]bool
}

func NewHub() *Hub {
	return &Hub{
		clients: make(map[*websocket.Conn]bool),
	}
}

func (h *Hub) HandleWS(conn *websocket.Conn) {
	h.clients[conn] = true
	log.Println("Client connected")

	defer func() {
		delete(h.clients, conn)
		conn.Close()
		log.Println("Client disconnected")
	}()

	for {
		var msg string
		if err := websocket.Message.Receive(conn, &msg); err != nil {
			return
		}

		for c := range h.clients {
			_ = websocket.Message.Send(c, msg)
		}
	}
}
