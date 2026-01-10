FROM golang:1.22-alpine AS builder

WORKDIR /app

RUN apk add --no-cache ca-certificates

COPY go.mod go.sum ./
RUN go mod download

COPY . .

RUN CGO_ENABLED=0 GOOS=linux GOARCH=amd64     go build -ldflags="-s -w" -o simplews

FROM gcr.io/distroless/base-debian12

WORKDIR /app

COPY --from=builder /app/simplews /app/simplews
COPY --from=builder /app/templates /app/templates
COPY --from=builder /app/static /app/static

EXPOSE 8080

USER nonroot:nonroot

ENTRYPOINT ["/app/simplews"]
