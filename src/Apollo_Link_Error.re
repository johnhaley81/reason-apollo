open Apollo_Types;

type networkError = {. "statusCode": int};

type graphqlError = {
  .
  "message": string,
  "locations": Js.Nullable.t(array(string)),
  "path": Js.Nullable.t(array(string)),
  "nodes": Js.Nullable.t(array(string)),
};

type executionResult = {
  .
  "errors": Js.Nullable.t(Js.Array.t(graphqlError)),
  "data": Js.Nullable.t(Js.Json.t),
};

/* TODO define all types */
type operation = {. "query": queryString};

/* TODO define subscription */
type subscription;

type errorResponse = {
  .
  "graphQLErrors": Js.Nullable.t(Js.Array.t(graphqlError)),
  "networkError": Js.Nullable.t(networkError),
  "response": Js.Nullable.t(executionResult),
  "operation": operation,
  "forward": operation => subscription,
};

[@bs.module "apollo-link-error"]
external make_: (errorResponse => unit) => apolloLink = "onError";

/**
 * CreateErrorLink
 * https://github.com/apollographql/apollo-link/tree/master/packages/apollo-link-error
 */
let make = errorHandler =>
  /* Instanciate a new error link object */
  make_(errorHandler);
