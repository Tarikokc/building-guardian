docker run -d \
  --name=grafana \
  -p 3000:3000 \
  -v /home/customer/Web_Server:/var/lib/grafana/sqlite \
  grafana/grafana