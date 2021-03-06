#include "service_layer_client_sync.h"

namespace chirpsystem {
void ServiceLayerClient::Init() {
  this->stub_ = ServiceLayer::NewStub(grpc::CreateChannel(
      kSERVICE_SERVER_ADDRESS, grpc::InsecureChannelCredentials()));
}

bool ServiceLayerClient::RegisterUser(const std::string& username) {
  RegisterRequest request;
  request.set_username(username);
  RegisterReply reply;
  ClientContext context;
  Status status = stub_->registeruser(&context, request, &reply);
  return status.ok();
}

bool ServiceLayerClient::Follow(const std::string& username,
                                const std::string& to_follow) {
  FollowRequest request;
  request.set_username(username);
  request.set_to_follow(to_follow);
  FollowReply reply;
  ClientContext context;
  Status status = stub_->follow(&context, request, &reply);
  return status.ok();
}

std::string ServiceLayerClient::SendChirp(const std::string& username,
                                          const std::string& text,
                                          const std::string& parent_id) {
  ChirpRequest request;
  request.set_username(username);
  request.set_text(text);
  request.set_parent_id(parent_id);
  ChirpReply reply;
  ClientContext context;
  Status status = stub_->chirp(&context, request, &reply);
  if (status.ok()) {
    return reply.chirp().id();
  }

  return "";
}

std::vector<Chirp> ServiceLayerClient::Read(const std::string& chirp_id) {
  ReadRequest request;
  request.set_chirp_id(chirp_id);
  ReadReply reply;
  ClientContext context;
  Status status = stub_->read(&context, request, &reply);
  std::vector<Chirp> chirps;
  if (status.ok()) {
    for (Chirp chirp : reply.chirps()) {
      chirps.push_back(chirp);
    }
  }
  return chirps;
}

bool ServiceLayerClient::Monitor(const std::string& username,
                                 std::function<void(Chirp)> handle_response) {
  MonitorRequest request;
  MonitorReply reply;
  ClientContext context;
  request.set_username(username);
  std::unique_ptr<ClientReader<MonitorReply>> reader(
      stub_->monitor(&context, request));
  // Receiving new chirps
  while (reader->Read(&reply)) {
    handle_response(reply.chirp());
  }
  Status status = reader->Finish();
  return status.ok();
}

bool ServiceLayerClient::Stream(const std::string& hashtag,
                                 std::function<void(Chirp)> handle_response) {
  StreamRequest request;
  StreamReply reply;
  ClientContext context;
  request.set_hashtag(hashtag);
  std::unique_ptr<ClientReader<StreamReply>> reader(
      stub_->stream(&context, request));
  // Receiving new chirps
  while (reader->Read(&reply)) {
    handle_response(reply.chirp());
  }
  Status status = reader->Finish();
  return status.ok();
}
}  // namespace chirpsystem