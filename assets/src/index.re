// [%raw
// {|
// const withAuthenticator =
//   BaseComponent => props =>
//     React.createElement("strong", null,
//       React.createElement(BaseComponent, props))
// |}];
// [@bs.val]
// external withAuthenticator:
//   (React.component('props), bool) => React.component('props) =
//   "app";

module WrappedApp = {
  /* [@bs.obj] external makeProps: (~name: 'name=?, unit) => {. "name": 'name} = ""; */
  include App;
  // let makeProps = App.makeProps;
  [@react.component]
  let make = App.make;
};
ReactDOMRe.renderToElementWithId(<WrappedApp />, "app");
