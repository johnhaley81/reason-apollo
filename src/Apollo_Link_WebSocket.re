/*
 apollo link ws
 */

[@bs.deriving abstract]
type webSocketLinkOptionsT = {
  [@bs.optional]
  reconnect: bool,
  [@bs.optional]
  connectionParams: Js.Json.t,
};

[@bs.deriving abstract]
type webSocketLinkT = {
  uri: string,
  options: webSocketLinkOptionsT,
};

[@bs.module "apollo-link-ws"] [@bs.new]
external webSocketLink: webSocketLinkT => Apollo.apolloLink = "WebSocketLink";

let webSocketLink = (~uri, ~reconnect=?, ~connectionParams=?, ()) =>
  webSocketLink(
    webSocketLinkT(
      ~uri,
      ~options=webSocketLinkOptionsT(~reconnect?, ~connectionParams?, ()),
    ),
  );
