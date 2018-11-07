open Apollo_Types;

/*
 apollo link ws
 */

[@bs.deriving abstract]
type webSocketLinkOptionsT = {
  [@bs.optional]
  reconnect: bool,
};

[@bs.deriving abstract]
type webSocketLinkT = {
  uri: string,
  options: webSocketLinkOptionsT,
};

/* bind apollo-link-ws */
[@bs.module "apollo-link-ws"] [@bs.new]
external make_: webSocketLinkT => apolloLink = "WebSocketLink";

let make = (~uri, ~reconnect=?, ()) =>
  make_(
    webSocketLinkT(~uri, ~options=webSocketLinkOptionsT(~reconnect?, ())),
  );
